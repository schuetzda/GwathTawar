#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderer.h"
#include "VulkanAPI/VulkanInstance.h"

namespace gwa {
	VulkanRenderer::VulkanRenderer(Window* window): m_window(window)
	{
	}
	void VulkanRenderer::init() const
	{
		
#ifdef GWA_DEBUG
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#else
		const std::vector<const char*> validationLayers;
#endif
		VulkanInstance instance(m_window->getAppTitle(), VK_MAKE_API_VERSION(1, 3, 0, 0), std::string("Gwa Engine"),
			VK_MAKE_API_VERSION(0, 1, 0, 0), VK_API_VERSION_1_3, &validationLayers);


	}
}
