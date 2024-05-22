#pragma once
#include <Core/Window.h>
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanSurface
	{

	public:
		VulkanSurface(Window * const window, VkInstance& vkInstance);
		~VulkanSurface();
		VkSurfaceKHR& getSurface() {
			return vkSurface;
		}
		void cleanup(VkInstance& vkInstance);
	private:
		VkSurfaceKHR vkSurface;
	};
}
