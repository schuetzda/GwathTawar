#pragma once
#include <Core/Window.h>
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanSurface
	{

	public:
		VulkanSurface(Window * window, VkInstance& vkInstance);
		~VulkanSurface();
		VkSurfaceKHR& getSurface() {
			return vkSurface;
		}
	private:
		VkSurfaceKHR vkSurface;
	};
}
