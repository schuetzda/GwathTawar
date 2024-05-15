#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>
namespace gwa 
{
	class PhysicalDevice
	{
	public:
		 PhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
	private:

		std::vector<VkPhysicalDevice> getPhysicalDeviceList(VkInstance& instance) const;
		bool checkPhysicalDeviceSuitable(const VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface) const;
		bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice) const;
		
		
		std::unique_ptr<VkPhysicalDevice> physicalDevice;
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};
}
