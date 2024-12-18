#pragma once
#include <core/Window.h>
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanSurface
	{

	public:
		VulkanSurface(const Window * const window, VkInstance vkInstance);
		~VulkanSurface();
		void cleanup(const VkInstance& vkInstance);

		VkSurfaceKHR vkSurface;
	private:
	};
}
