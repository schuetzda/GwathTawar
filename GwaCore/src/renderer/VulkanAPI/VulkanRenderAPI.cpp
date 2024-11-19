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
		m_graphicsPipeline = std::make_unique<VulkanPipeline>(m_logicalDevice->getLogicalDevice(), sizeof(Vertex), offset, m_renderPass->getRenderPass(), m_swapchain->getSwapchainExtent(),m_pushConstant->getPushConstantRange(), m_descriptorSetLayout->getDescriptorSetLayout());

		m_depthBufferImage = std::make_unique<VulkanImage>(m_logicalDevice->getLogicalDevice(), m_physicalDevice->getPhysicalDevice(), m_swapchain->getSwapchainExtent(),
			m_renderPass->getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthBufferImageView = std::make_unique<VulkanImageView>(m_logicalDevice->getLogicalDevice(), m_depthBufferImage->getImage(), m_renderPass->getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

		m_swapchainFramebuffers = std::make_unique<VulkanSwapchainFramebuffers>(m_logicalDevice->getLogicalDevice(), m_swapchain->getSwapchainImages(), m_renderPass->getRenderPass(), m_depthBufferImageView->getImageView(),
			m_swapchain->getSwapchainExtent());

		m_graphicsCommandPool = std::make_unique<VulkanCommandPool>(m_logicalDevice->getLogicalDevice(), m_physicalDevice->getPhysicalDevice(), m_surface->getSurface());
	}

	void VulkanRenderAPI::shutdown() {
		m_graphicsCommandPool->cleanup(m_logicalDevice->getLogicalDevice());
		m_swapchainFramebuffers->cleanup(m_logicalDevice->getLogicalDevice());
		m_depthBufferImageView->cleanup(m_logicalDevice->getLogicalDevice());
		m_depthBufferImage->cleanup(m_logicalDevice->getLogicalDevice());
		m_graphicsPipeline->cleanup(m_logicalDevice->getLogicalDevice());
		m_descriptorSetLayout->cleanup(m_logicalDevice->getLogicalDevice());
		m_renderPass->cleanup(m_logicalDevice->getLogicalDevice());
		m_swapchain->cleanup(m_logicalDevice->getLogicalDevice());
		m_surface->cleanup(m_instance->getVkInstance());
		m_logicalDevice->cleanup();
		m_instance->cleanup();
	}
}
