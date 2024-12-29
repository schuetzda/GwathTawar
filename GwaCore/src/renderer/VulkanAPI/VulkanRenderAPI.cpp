#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderAPI.h"
#include <array>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>

namespace gwa {
	void VulkanRenderAPI::init(const Window *  window) 
	{
		
#ifdef GWA_DEBUG
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#else
		const std::vector<const char*> validationLayers;
#endif

		// Create Vulkan instance and enable/ disable ValidationLayers
		m_instance.init(window->getAppTitle(), std::string("Gwa Engine"), VK_MAKE_API_VERSION(1, 3, 0, 0), 
			VK_MAKE_API_VERSION(0, 1, 0, 0), VK_API_VERSION_1_3, &validationLayers);

		m_device.init(window, m_instance.getVkInstance(), deviceExtensions);

		WindowSize framebufferSize = window->getFramebufferSize();
		m_swapchain.init(&m_device, framebufferSize.width, framebufferSize.height);

		m_renderPass.init(&m_device, m_swapchain.getImageFormat());

		m_descriptorSetLayout.init(m_device.getLogicalDevice());

		m_pushConstant.init(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4)); //TODO

		std::vector<uint32_t> offset = { offsetof(Vertex, position), offsetof(Vertex, color) }; //TODO move
		m_graphicsPipeline.init(m_device.getLogicalDevice(), sizeof(Vertex), offset, m_renderPass.getRenderPass(), m_swapchain.getSwapchainExtent(), m_pushConstant.getRange(), m_descriptorSetLayout.getDescriptorSetLayout());

		m_depthBufferImage = std::make_unique<VulkanImage>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, m_swapchain->m_vkSwapchainExtent,
			m_renderPass->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthBufferImageView = std::make_unique<VulkanImageView>(m_logicalDevice->logicalDevice, m_depthBufferImage->image, m_renderPass->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		m_swapchainFramebuffers = std::make_unique<VulkanSwapchainFramebuffers>(m_logicalDevice->logicalDevice, m_swapchain->m_swapchainImages, m_renderPass->m_vkRenderPass, m_depthBufferImageView->imageView,
			m_swapchain->m_vkSwapchainExtent);

		m_graphicsCommandPool = std::make_unique<VulkanCommandPool>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, m_surface->vkSurface);
		
		m_meshBuffers = std::make_unique<VulkanMeshBuffers>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice);
		uboViewProj.projection = glm::perspective(glm::radians(45.0f), (float)m_swapchain->m_vkSwapchainExtent.width / (float)m_swapchain->m_vkSwapchainExtent.height, 0.1f, 100.0f);
		uboViewProj.view = glm::lookAt(glm::vec3(3.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		uboViewProj.projection[1][1] *= -1;	// Invert the y-axis because difference between OpenGL and Vulkan standard
				
		for (int i=0; i< renderDataManager.getRenderDataToSubmit().size(); ++i)
		{
			renderDataManager.meshesToBind[i]->meshBufferIndex = m_meshBuffers->addBuffer(renderDataManager.getRenderDataToSubmit()[i].vertices, renderDataManager.getRenderDataToSubmit()[i].indices, m_logicalDevice->graphicsQueue, m_graphicsCommandPool->commandPool);
		}
		
		m_graphicsCommandBuffer = std::make_unique<VulkanCommandBuffers>(m_logicalDevice->logicalDevice, m_graphicsCommandPool->commandPool, MAX_FRAMES_IN_FLIGHT);

		m_mvpUniformBuffers = std::make_unique<VulkanUniformBuffers>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, sizeof(UboViewProj), m_swapchain->m_swapchainImages.size());

		m_descriptorSet = std::make_unique<VulkanDescriptorSet>(m_logicalDevice->logicalDevice, m_descriptorSetLayout->m_vkDescriptorSetLayout, m_mvpUniformBuffers->uniformBuffers,
			MAX_FRAMES_IN_FLIGHT, sizeof(UboViewProj));

		m_renderFinished = std::make_unique<VulkanSemaphore>(m_logicalDevice->logicalDevice, MAX_FRAMES_IN_FLIGHT);
		m_imageAvailable = std::make_unique<VulkanSemaphore>(m_logicalDevice->logicalDevice, MAX_FRAMES_IN_FLIGHT);
		m_drawFences = std::make_unique<VulkanFence>(m_logicalDevice->logicalDevice, MAX_FRAMES_IN_FLIGHT);
	}

	void VulkanRenderAPI::draw(const Window *  window)
	{
		const VkDevice logicalDevice = m_logicalDevice->logicalDevice;
		const VkPhysicalDevice physicalDevice = m_physicalDevice->physicalDevice;

		WindowSize framebufferSize = window->getFramebufferSize();
		const std::vector<VkSemaphore>& imageAvailabe = m_imageAvailable->vkSemaphores;
		const std::vector<VkSemaphore>& renderFinished = m_renderFinished->vkSemaphores;

		vkWaitForFences(logicalDevice, 1, &m_drawFences->fences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(logicalDevice, m_swapchain->m_vkSwapchain, std::numeric_limits<uint64_t>::max(), imageAvailabe[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			vkDeviceWaitIdle(logicalDevice);
			m_swapchain->recreateSwapchain(logicalDevice, physicalDevice, m_surface->vkSurface, framebufferSize.width, framebufferSize.height);
			m_swapchainFramebuffers->recreateSwapchain(logicalDevice, m_swapchain->m_swapchainImages, m_renderPass->m_vkRenderPass,
				m_depthBufferImageView->imageView, m_swapchain->m_vkSwapchainExtent);
			return;
		}
		else
		{
			assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
		}

		m_mvpUniformBuffers->updateUniformBuffers(logicalDevice, imageIndex, sizeof(UboViewProj), &uboViewProj);
		vkResetFences(logicalDevice, 1, &m_drawFences->fences[currentFrame]);
		vkResetCommandBuffer(m_graphicsCommandBuffer->commandBuffers[currentFrame], 0);

		recordCommands(imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;						// Nummer of semaphores to wait on
		submitInfo.pWaitSemaphores = &imageAvailabe[currentFrame];			// List of Semphores to wait on
		std::array<VkPipelineStageFlags,1> waitStages = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};
		submitInfo.pWaitDstStageMask = waitStages.data();				//Stages to check semaphores
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_graphicsCommandBuffer->commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;					// Number of semaphores to signal
		submitInfo.pSignalSemaphores = &renderFinished[currentFrame];			// Semaphores to signal when command buffer finishes

		result = vkQueueSubmit(m_logicalDevice->graphicsQueue, 1, &submitInfo, m_drawFences->fences[currentFrame]);
		assert(result == VK_SUCCESS);

		// 3. Present image to screen when it has signaled finished rendering
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinished[currentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapchain->m_vkSwapchain;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(m_logicalDevice->presentationQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			m_swapchain->recreateSwapchain(logicalDevice, physicalDevice, m_surface->vkSurface, framebufferSize.width, framebufferSize.height);
			m_swapchainFramebuffers->recreateSwapchain(logicalDevice, m_swapchain->m_swapchainImages, m_renderPass->m_vkRenderPass,
				m_depthBufferImageView->imageView, m_swapchain->m_vkSwapchainExtent);
		}
		else
		{
			assert(result == VK_SUCCESS);
		}
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	}

	void VulkanRenderAPI::shutdown() {
		const VkDevice cur_logicalDevice = m_logicalDevice->logicalDevice;

		vkDeviceWaitIdle(cur_logicalDevice);
		
		m_meshBuffers->cleanup();
		m_drawFences->cleanup(cur_logicalDevice);
		m_renderFinished->cleanup(cur_logicalDevice);
		m_imageAvailable->cleanup(cur_logicalDevice);
		m_descriptorSet->cleanup(cur_logicalDevice);
		m_mvpUniformBuffers->cleanup(cur_logicalDevice);
		m_graphicsCommandPool->cleanup(cur_logicalDevice);
		m_swapchainFramebuffers->cleanup(cur_logicalDevice);
		m_depthBufferImageView->cleanup(cur_logicalDevice);
		m_depthBufferImage->cleanup(cur_logicalDevice);
		m_graphicsPipeline->cleanup(cur_logicalDevice);
		m_descriptorSetLayout->cleanup(cur_logicalDevice);
		m_renderPass->cleanup(cur_logicalDevice);
		m_swapchain->cleanup(cur_logicalDevice);
		m_surface->cleanup(m_instance.vkInstance_);
		m_logicalDevice->cleanup();
		m_instance.cleanup();
	}
	void VulkanRenderAPI::recordCommands(uint32_t imageIndex)
	{
		VkExtent2D extent = m_swapchain->m_vkSwapchainExtent;
		m_graphicsCommandBuffer->beginCommandBuffer(currentFrame, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
		m_graphicsCommandBuffer->beginRenderPass(m_renderPass->m_vkRenderPass, extent,
			m_swapchainFramebuffers->swapchainFramebuffers[imageIndex], currentFrame);
		m_graphicsCommandBuffer->bindPipeline(m_graphicsPipeline->graphicsPipeline_, currentFrame);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_graphicsCommandBuffer->setViewport(viewport, currentFrame);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		m_graphicsCommandBuffer->setScissor(scissor, currentFrame);

		for (Mesh mesh: m_meshes)
		{
			VulkanMeshBuffers::MeshBufferData meshData = m_meshBuffers->getMeshBufferData(mesh.meshBufferIndex);
			//TODO correct return types
			VkDeviceSize offsets[] = { 0 };
			VkBuffer vertexBuffers[] = { meshData.vertexBuffer };
			m_graphicsCommandBuffer->bindVertexBuffer(vertexBuffers, offsets, currentFrame);
			m_graphicsCommandBuffer->bindIndexBuffer(meshData.indexBuffer, currentFrame);

			m_graphicsCommandBuffer->pushConstants(m_graphicsPipeline->pipelineLayout_, VK_SHADER_STAGE_VERTEX_BIT, currentFrame, &mesh.modelMatrix);
			m_graphicsCommandBuffer->bindDescriptorSet(m_descriptorSet->descriptorSets[currentFrame], m_graphicsPipeline->pipelineLayout_, currentFrame);

			m_graphicsCommandBuffer->drawIndexed(meshData.indexCount, currentFrame);
		}
		m_graphicsCommandBuffer->endRenderPass(currentFrame);
		m_graphicsCommandBuffer->endCommandBuffer(currentFrame);
	}
	void VulkanRenderAPI::updateModel(int modelId, const glm::mat4& newModel)
	{
		m_meshes[modelId].modelMatrix = newModel;
	}
}
