#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderAPI.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

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
	}

	void VulkanRenderAPI::shutdown() {
		m_swapchain->cleanup(m_logicalDevice->getLogicalDevice());
		m_surface->cleanup(m_instance->getVkInstance());
		m_logicalDevice->cleanup();
		m_instance->cleanup();
	}
}
