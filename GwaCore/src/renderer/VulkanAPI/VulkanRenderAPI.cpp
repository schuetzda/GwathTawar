#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderAPI.h"
#include <array>
#include <stdexcept>

namespace gwa {
	void VulkanRenderAPI::init(Window * window) 
	{
		
#ifdef GWA_DEBUG
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#else
		const std::vector<const char*> validationLayers;
#endif

		// Create Vulkan instance and enable/ disable ValidationLayers
		m_instance = std::make_unique<VulkanInstance>(window->getAppTitle(), VK_MAKE_API_VERSION(1, 3, 0, 0), std::string("Gwa Engine"),
			VK_MAKE_API_VERSION(0, 1, 0, 0), VK_API_VERSION_1_3, &validationLayers);

		// Create Vulkan surface
		m_surface = std::make_unique<VulkanSurface>(window, m_instance->vkInstance);

		m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(m_instance->vkInstance, m_surface->vkSurface, deviceExtensions);

		m_logicalDevice = std::make_unique<VulkanLogicalDevice>(m_physicalDevice->physicalDevice, m_surface->vkSurface, deviceExtensions);

		WindowSize framebufferSize = window->getFramebufferSize();
		m_swapchain = std::make_unique<VulkanSwapchain>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, m_surface->vkSurface, framebufferSize.width, framebufferSize.height);

		m_renderPass = std::make_unique<VulkanRenderPass>(m_physicalDevice->physicalDevice, m_logicalDevice->logicalDevice, m_swapchain->vkSwapchainImageFormat);

		m_descriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>(m_logicalDevice->logicalDevice);

		m_pushConstant = std::make_unique<VulkanPushConstant>(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Model));

		std::vector<uint32_t> offset = { offsetof(Vertex, position), offsetof(Vertex, color) };
		m_graphicsPipeline = std::make_unique<VulkanPipeline>(m_logicalDevice->logicalDevice, sizeof(Vertex), offset, m_renderPass->vkRenderPass, m_swapchain->vkSwapchainExtent, m_pushConstant->getPushConstantRange(), m_descriptorSetLayout->vkDescriptorSetLayout);

		m_depthBufferImage = std::make_unique<VulkanImage>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, m_swapchain->vkSwapchainExtent,
			m_renderPass->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthBufferImageView = std::make_unique<VulkanImageView>(m_logicalDevice->logicalDevice, m_depthBufferImage->image, m_renderPass->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		m_swapchainFramebuffers = std::make_unique<VulkanSwapchainFramebuffers>(m_logicalDevice->logicalDevice, m_swapchain->swapchainImages, m_renderPass->vkRenderPass, m_depthBufferImageView->imageView,
			m_swapchain->vkSwapchainExtent);

		m_graphicsCommandPool = std::make_unique<VulkanCommandPool>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, m_surface->vkSurface);

		//--TODO move to own implementation
		std::vector<Vertex> meshVertices1 = {
		{ { -0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },	// 0
		{ { -0.4, -0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },	    // 1
		{ { 0.4, -0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },    // 2
		{ { 0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },   // 3
		};

		std::vector<Vertex> meshVertices2 = {
			{ { -0.25, 0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },	// 0
			{ { -0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },	    // 1
			{ { 0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },    // 2
			{ { 0.25, 0.6, 0.0 },{ 0.0f, 0.0f, 11.0f } },   // 3
		};

		//Index Data
		std::vector<uint32_t> meshIndices = {
			0, 1, 2,
			2, 3, 0
		};

		MeshBuffer buffer1 = MeshBuffer(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, std::span<Vertex>(meshVertices1), std::span<uint32_t>(meshIndices),
			m_logicalDevice->graphicsQueue, m_graphicsCommandPool->commandPool);

		MeshBuffer buffer2 = MeshBuffer(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, std::span<Vertex>(meshVertices2), std::span<uint32_t>(meshIndices),
			m_logicalDevice->graphicsQueue, m_graphicsCommandPool->commandPool);

		meshes.push_back(buffer1);
		meshes.push_back(buffer2);

		m_graphicsCommandBuffer = std::make_unique<VulkanCommandBuffers>(m_logicalDevice->logicalDevice, m_graphicsCommandPool->commandPool, MAX_FRAMES_IN_FLIGHT);

		m_mvpUniformBuffers = std::make_unique<VulkanUniformBuffers>(m_logicalDevice->logicalDevice, m_physicalDevice->physicalDevice, sizeof(UboViewProj), MAX_FRAMES_IN_FLIGHT);

		m_descriptorSet = std::make_unique<VulkanDescriptorSet>(m_logicalDevice->logicalDevice, m_descriptorSetLayout->vkDescriptorSetLayout, m_mvpUniformBuffers->uniformBuffers,
			MAX_FRAMES_IN_FLIGHT, sizeof(UboViewProj));

		m_renderFinished = std::make_unique<VulkanSemaphore>(m_logicalDevice->logicalDevice, MAX_FRAMES_IN_FLIGHT);
		m_imageAvailable = std::make_unique<VulkanSemaphore>(m_logicalDevice->logicalDevice, MAX_FRAMES_IN_FLIGHT);
		m_drawFences = std::make_unique<VulkanFence>(m_logicalDevice->logicalDevice, MAX_FRAMES_IN_FLIGHT);
	}

	void VulkanRenderAPI::draw(Window * window)
	{
		const VkDevice logicalDevice = m_logicalDevice->logicalDevice;
		const VkPhysicalDevice physicalDevice = m_physicalDevice->physicalDevice;

		WindowSize framebufferSize = window->getFramebufferSize();
		const std::vector<VkSemaphore>& imageAvailabe = m_imageAvailable->vkSemaphores;
		const std::vector<VkSemaphore>& renderFinished = m_renderFinished->vkSemaphores;

		vkWaitForFences(logicalDevice, 1, &m_drawFences->fences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(logicalDevice, m_swapchain->vkSwapchain, std::numeric_limits<uint64_t>::max(), imageAvailabe[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			vkDeviceWaitIdle(logicalDevice);
			m_swapchain->recreateSwapchain(logicalDevice, physicalDevice, m_surface->vkSurface, framebufferSize.width, framebufferSize.height);
			m_swapchainFramebuffers->recreateSwapchain(logicalDevice, m_swapchain->swapchainImages, m_renderPass->vkRenderPass,
				m_depthBufferImageView->imageView, m_swapchain->vkSwapchainExtent);
			return;
		}
		else
		{
			assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
		}
		vkResetFences(logicalDevice, 1, &m_drawFences->fences[currentFrame]);
		vkResetCommandBuffer(m_graphicsCommandBuffer->commandBuffers[currentFrame], 0);

		recordCommands(currentFrame);

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
		presentInfo.pSwapchains = &m_swapchain->vkSwapchain;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(m_logicalDevice->presentationQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			m_swapchain->recreateSwapchain(logicalDevice, physicalDevice, m_surface->vkSurface, framebufferSize.width, framebufferSize.height);
			m_swapchainFramebuffers->recreateSwapchain(logicalDevice, m_swapchain->swapchainImages, m_renderPass->vkRenderPass,
				m_depthBufferImageView->imageView, m_swapchain->vkSwapchainExtent);
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
		
		for (MeshBuffer mesh : meshes)
		{
			mesh.cleanup(cur_logicalDevice);
		}

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
		m_surface->cleanup(m_instance->vkInstance);
		m_logicalDevice->cleanup();
		m_instance->cleanup();
	}
	void VulkanRenderAPI::recordCommands(const int currentFrame)
	{
		VkExtent2D extent = m_swapchain->vkSwapchainExtent;
		m_graphicsCommandBuffer->beginCommandBuffer(currentFrame);
		m_graphicsCommandBuffer->beginRenderPass(m_renderPass->vkRenderPass, extent,
			m_swapchainFramebuffers->swapchainFramebuffers[currentFrame], currentFrame);
		m_graphicsCommandBuffer->bindPipeline(m_graphicsPipeline->graphicsPipeline, currentFrame);

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

		for (MeshBuffer mesh : meshes)
		{
			VkBuffer vertexBuffers[] = { mesh.vertexBuffer.buffer };			// Buffers to bind
			VkDeviceSize offsets[] = { 0 };
			m_graphicsCommandBuffer->bindVertexBuffer(vertexBuffers, offsets, currentFrame);
			
		}
	}
}
