#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderAPI.h"
#include <array>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include "ecs/Registry.h"
#include "resources/RenderObjects.h"

namespace gwa {
	void VulkanRenderAPI::init(const Window *  window, gwa::ntity::Registry& registry) 
	{
		
#ifdef GWA_DEBUG
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#else
		const std::vector<const char*> validationLayers;
#endif

		// Create Vulkan instance and enable/ disable ValidationLayers
		m_instance = VulkanInstance(window->getAppTitle(), std::string("Gwa Engine"), VK_MAKE_API_VERSION(1, 3, 0, 0), 
			VK_MAKE_API_VERSION(0, 1, 0, 0), VK_API_VERSION_1_3, &validationLayers);

		m_device = VulkanDevice(window, m_instance.getVkInstance(), deviceExtensions);

		WindowSize framebufferSize = window->getFramebufferSize();
		m_swapchain = VulkanSwapchain(&m_device, framebufferSize.width, framebufferSize.height);

		m_renderPass = VulkanRenderPass(&m_device, m_swapchain.getImageFormat());

		m_descriptorSetLayout = VulkanDescriptorSetLayout(m_device.getLogicalDevice());

		m_pushConstant = VulkanPushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4)); //TODO

		std::vector<uint32_t> offset = { 0, 0 }; //TODO move
		m_graphicsPipeline = VulkanPipeline(m_device.getLogicalDevice(), sizeof(glm::vec3), offset, m_renderPass.getRenderPass(), m_swapchain.getSwapchainExtent(), m_pushConstant.getRange(), m_descriptorSetLayout.getDescriptorSetLayout());

		m_depthBufferImage = VulkanImage(&m_device, m_swapchain.getSwapchainExtent(),m_renderPass.getDepthFormat(),
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_depthBufferImageView = VulkanImageView(m_device.getLogicalDevice(), m_depthBufferImage.getImage(), m_renderPass.getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

		m_swapchainFramebuffers = VulkanSwapchainFramebuffers(m_device.getLogicalDevice(), m_swapchain.getSwapchainImages(), 
			m_renderPass.getRenderPass(), m_depthBufferImageView.getImageView(), m_swapchain.getSwapchainExtent());

		m_graphicsCommandPool = VulkanCommandPool(&m_device);
		
		m_meshBuffers = VulkanMeshBuffers(m_device.getLogicalDevice(), m_device.getPhysicalDevice());
		uboViewProj.projection = glm::perspective(glm::radians(45.0f), (float)m_swapchain.getSwapchainExtent().width / (float)m_swapchain.getSwapchainExtent().height, 0.1f, 10000.0f);

		uboViewProj.projection[1][1] *= -1;	// Invert the y-axis because difference between OpenGL and Vulkan standard
		
		for (int i=0; i < registry.getComponentCount<TexturedMeshBufferMemory>(); i++)
		{
			TexturedMeshBufferMemory const * meshBufferMemory = registry.getComponent<TexturedMeshBufferMemory>(i);
			const uint32_t id = m_meshBuffers.addBuffer(*meshBufferMemory->vertices, *meshBufferMemory->normals, *meshBufferMemory->indices, m_device.getGraphicsQueue(), m_graphicsCommandPool.getCommandPool());
			uint32_t entityID = registry.registerEntity();
			TexturedMeshRenderObject renderObject;
			renderObject.bufferID = id;
			registry.addComponent<TexturedMeshRenderObject>(entityID, std::move(renderObject));
		}
		registry.flushComponents<TexturedMeshBufferMemory>();
		
		m_graphicsCommandBuffer = VulkanCommandBuffers(m_device.getLogicalDevice(), m_graphicsCommandPool.getCommandPool(), maxFramesInFlight_);

		m_mvpUniformBuffers = VulkanUniformBuffers(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), sizeof(UboViewProj), m_swapchain.getSwapchainImagesSize());

		m_descriptorSet = VulkanDescriptorSet(m_device.getLogicalDevice(), m_descriptorSetLayout.getDescriptorSetLayout(), m_mvpUniformBuffers.getUniformBuffers(),
			maxFramesInFlight_, sizeof(UboViewProj));

		m_renderFinished = VulkanSemaphore(m_device.getLogicalDevice(), maxFramesInFlight_);
		m_imageAvailable = VulkanSemaphore(m_device.getLogicalDevice(), maxFramesInFlight_);
		m_drawFences = VulkanFence(m_device.getLogicalDevice(), maxFramesInFlight_);
	}

	void VulkanRenderAPI::draw(const Window *  window, gwa::ntity::Registry& registry)
	{
		WindowSize framebufferSize = window->getFramebufferSize();
		const std::vector<VkSemaphore>& imageAvailabe = m_imageAvailable.getSemaphores();
		const std::vector<VkSemaphore>& renderFinished = m_renderFinished.getSemaphores();

		vkWaitForFences(m_device.getLogicalDevice(), 1, &m_drawFences.getFences()[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_device.getLogicalDevice(), m_swapchain.getSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailabe[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			vkDeviceWaitIdle(m_device.getLogicalDevice());
			m_swapchain.recreateSwapchain(framebufferSize.width, framebufferSize.height);
			m_swapchainFramebuffers.recreateSwapchain(m_swapchain.getSwapchainImages(), m_renderPass.getRenderPass(),
				m_depthBufferImageView.getImageView(), m_swapchain.getSwapchainExtent());
			return;
		}
		else
		{
			assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
		}

		m_mvpUniformBuffers.updateUniformBuffers(imageIndex, sizeof(UboViewProj), &uboViewProj);
		vkResetFences(m_device.getLogicalDevice(), 1, &m_drawFences.getFences()[currentFrame]);
		vkResetCommandBuffer(*m_graphicsCommandBuffer.getCommandBuffer(currentFrame), 0);

		recordCommands(imageIndex, registry);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;						// Nummer of semaphores to wait on
		submitInfo.pWaitSemaphores = &imageAvailabe[currentFrame];			// List of Semphores to wait on
		std::array<VkPipelineStageFlags,1> waitStages = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};
		submitInfo.pWaitDstStageMask = waitStages.data();				//Stages to check semaphores
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = m_graphicsCommandBuffer.getCommandBuffer(currentFrame);
		submitInfo.signalSemaphoreCount = 1;					// Number of semaphores to signal
		submitInfo.pSignalSemaphores = &renderFinished[currentFrame];			// Semaphores to signal when command buffer finishes

		result = vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_drawFences.getFences()[currentFrame]);
		assert(result == VK_SUCCESS);

		// 3. Present image to screen when it has signaled finished rendering
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinished[currentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = m_swapchain.getSwapchainPtr();
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(m_device.getPresentationQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			m_swapchain.recreateSwapchain(framebufferSize.width, framebufferSize.height);
			m_swapchainFramebuffers.recreateSwapchain(m_swapchain.getSwapchainImages(), m_renderPass.getRenderPass(),
				m_depthBufferImageView.getImageView(), m_swapchain.getSwapchainExtent());
		}
		else
		{
			assert(result == VK_SUCCESS);
		}
		currentFrame = (currentFrame + 1) % maxFramesInFlight_;

	}

	void VulkanRenderAPI::shutdown() {
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		
		m_meshBuffers.cleanup();
		m_drawFences.cleanup();
		m_renderFinished.cleanup();
		m_imageAvailable.cleanup();
		m_descriptorSet.cleanup();
		m_mvpUniformBuffers.cleanup();
		m_graphicsCommandPool.cleanup();
		m_swapchainFramebuffers.cleanup();
		m_depthBufferImageView.cleanup();
		m_depthBufferImage.cleanup();
		m_graphicsPipeline.cleanup();
		m_descriptorSetLayout.cleanup();
		m_renderPass.cleanup();
		m_swapchain.cleanup();
		m_device.cleanup();
		m_instance.cleanup();
	}
	void VulkanRenderAPI::recordCommands(uint32_t imageIndex, gwa::ntity::Registry& registry)
	{
		VkExtent2D extent = m_swapchain.getSwapchainExtent();
		m_graphicsCommandBuffer.beginCommandBuffer(currentFrame, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
		m_graphicsCommandBuffer.beginRenderPass(m_renderPass.getRenderPass(), extent,
			m_swapchainFramebuffers.getFramebuffers()[imageIndex], currentFrame);
		m_graphicsCommandBuffer.bindPipeline(m_graphicsPipeline.getPipeline(), currentFrame);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_graphicsCommandBuffer.setViewport(viewport, currentFrame);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		m_graphicsCommandBuffer.setScissor(scissor, currentFrame);
		
		for (uint32_t i=0; i < registry.getComponentCount<TexturedMeshRenderObject>(); i++)
		{
			TexturedMeshRenderObject* renderObject = registry.getComponent<TexturedMeshRenderObject>(i);
			VulkanMeshBuffers::MeshBufferData meshData = m_meshBuffers.getMeshBufferData(renderObject->bufferID);
			//TODO correct return types
			VkDeviceSize offsets[2] = { 0, 0 };
			VkBuffer vertexBuffers[2] = { meshData.vertexBuffer, meshData.normalBuffer };
			m_graphicsCommandBuffer.bindVertexBuffer(vertexBuffers,2, offsets, currentFrame);
			m_graphicsCommandBuffer.bindIndexBuffer(meshData.indexBuffer, currentFrame);

			m_graphicsCommandBuffer.pushConstants(m_graphicsPipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, currentFrame, &renderObject->modelMatrix);
			m_graphicsCommandBuffer.bindDescriptorSet(m_descriptorSet.getDescriptorSets()[currentFrame], m_graphicsPipeline.getPipelineLayout(), currentFrame);

			m_graphicsCommandBuffer.drawIndexed(meshData.indexCount, currentFrame);
		}
		
		
		m_graphicsCommandBuffer.endRenderPass(currentFrame);
		m_graphicsCommandBuffer.endCommandBuffer(currentFrame);
	}
}
