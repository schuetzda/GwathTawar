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
		VkSurfaceKHR getSurface() const 
		{
			return vkSurface;
		}
		void cleanup(const VkInstance& vkInstance);
	private:
		VkSurfaceKHR vkSurface;
	};
}
