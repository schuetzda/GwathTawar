#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderAPI.h"

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
		m_surface = std::make_unique<VulkanSurface>(window, m_instance->getVkInstance());

		m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(m_instance->getVkInstance(), m_surface->getSurface(), deviceExtensions);

		m_logicalDevice = std::make_unique<VulkanLogicalDevice>(m_physicalDevice->getPhysicalDevice(), m_surface->getSurface(), deviceExtensions);

		WindowSize framebufferSize = window->getFramebufferSize();
		m_swapchain = std::make_unique<VulkanSwapchain>(m_physicalDevice->getPhysicalDevice(), m_logicalDevice->getLogicalDevice(), m_surface->getSurface(), framebufferSize.width, framebufferSize.height);

		m_renderPass = std::make_unique<VulkanRenderPass>(m_physicalDevice->getPhysicalDevice(), m_logicalDevice->getLogicalDevice(), m_swapchain->getSwapchainFormat());

		m_descriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>(m_logicalDevice->getLogicalDevice());

		m_pushConstant = std::make_unique<VulkanPushConstant>(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Model));

		std::vector<uint32_t> offset = { offsetof(Vertex, pos), offsetof(Vertex, col) };
		m_graphicsPipeline = std::make_unique<VulkanPipeline>(m_logicalDevice->getLogicalDevice(), sizeof(Vertex), offset, m_renderPass->getRenderPass(), m_swapchain->getSwapchainExtent(), m_pushConstant->getPushConstantRange(), m_descriptorSetLayout->getDescriptorSetLayout());

		m_depthBufferImage = std::make_unique<VulkanImage>(m_logicalDevice->getLogicalDevice(), m_physicalDevice->getPhysicalDevice(), m_swapchain->getSwapchainExtent(),
			m_renderPass->getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthBufferImageView = std::make_unique<VulkanImageView>(m_logicalDevice->getLogicalDevice(), m_depthBufferImage->getImage(), m_renderPass->getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

		m_swapchainFramebuffers = std::make_unique<VulkanSwapchainFramebuffers>(m_logicalDevice->getLogicalDevice(), m_swapchain->getSwapchainImages(), m_renderPass->getRenderPass(), m_depthBufferImageView->getImageView(),
			m_swapchain->getSwapchainExtent());

		m_graphicsCommandPool = std::make_unique<VulkanCommandPool>(m_logicalDevice->getLogicalDevice(), m_physicalDevice->getPhysicalDevice(), m_surface->getSurface());
		m_graphicsCommandBuffer = std::make_unique<VulkanCommandBuffers>(m_logicalDevice->getLogicalDevice(), m_graphicsCommandPool->getCommandPool(), MAX_FRAMES_IN_FLIGHT);

		m_mvpUniformBuffers = std::make_unique<VulkanUniformBuffers>(m_logicalDevice->getLogicalDevice(), m_physicalDevice->getPhysicalDevice(), sizeof(UboViewProj), m_swapchainImageCount);

		m_descriptorPool = std::make_unique<VulkanDescriptorPool>(m_logicalDevice->getLogicalDevice(), m_mvpUniformBuffers->getUniformBuffers(), m_swapchainImageCount);
		m_descriptorSet = std::make_unique<VulkanDescriptorSet>(m_logicalDevice->getLogicalDevice(), m_descriptorSetLayout->getDescriptorSetLayout(), m_descriptorPool->getDescriptorPool(),
												m_mvpUniformBuffers->getUniformBuffers(), m_swapchainImageCount, sizeof(UboViewProj));

		m_renderFinished = std::make_unique<VulkanSemaphore>(m_logicalDevice->getLogicalDevice(), MAX_FRAMES_IN_FLIGHT);
		m_imageAvailable = std::make_unique<VulkanSemaphore>(m_logicalDevice->getLogicalDevice(), MAX_FRAMES_IN_FLIGHT);
		m_drawFences = std::make_unique<VulkanFence>(m_logicalDevice->getLogicalDevice(), MAX_FRAMES_IN_FLIGHT);
	}

	void VulkanRenderAPI::draw()
	{
		const VkDevice logicalDevice = m_logicalDevice->getLogicalDevice();
		const VkSwapchainKHR& swapchain = m_swapchain->getSwapchain();

		const std::vector<VkFence>& drawFences = m_drawFences->getFences();
		const std::vector<VkSemaphore>& imageAvailabe = m_imageAvailable->getSemaphores();
		const std::vector<VkSemaphore>& renderFinished = m_renderFinished->getSemaphores();

		vkWaitForFences(logicalDevice, 1, &drawFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, std::numeric_limits<uint64_t>::max(), imageAvailabe[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
		
		}
	}

	void VulkanRenderAPI::shutdown() {
		const VkDevice cur_logicalDevice = m_logicalDevice->getLogicalDevice();

		m_drawFences->cleanup(cur_logicalDevice);
		m_renderFinished->cleanup(cur_logicalDevice);
		m_imageAvailable->cleanup(cur_logicalDevice);
		m_descriptorPool->cleanup(cur_logicalDevice);
		m_mvpUniformBuffers->cleanup(cur_logicalDevice);
		m_graphicsCommandPool->cleanup(cur_logicalDevice);
		m_swapchainFramebuffers->cleanup(cur_logicalDevice);
		m_depthBufferImageView->cleanup(cur_logicalDevice);
		m_depthBufferImage->cleanup(cur_logicalDevice);
		m_graphicsPipeline->cleanup(cur_logicalDevice);
		m_descriptorSetLayout->cleanup(cur_logicalDevice);
		m_renderPass->cleanup(cur_logicalDevice);
		m_swapchain->cleanup(cur_logicalDevice);
		m_surface->cleanup(m_instance->getVkInstance());
		m_logicalDevice->cleanup();
		m_instance->cleanup();
	}
}
