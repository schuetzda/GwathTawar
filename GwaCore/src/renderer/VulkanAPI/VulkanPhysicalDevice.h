#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>
#include <string>
namespace gwa 
{
	class VulkanPhysicalDevice
	{
	public:
		 VulkanPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface, std::shared_ptr<const std::vector<const char*>> deviceExtensions);
		 VkPhysicalDevice& getPhysicalDevice() {
			 return physicalDevice;
		 }
	private:

		std::vector<VkPhysicalDevice> getPhysicalDeviceList(VkInstance& instance) const;
		bool checkPhysicalDeviceSuitable(const VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface) const;
		bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice) const;
		
		
		VkPhysicalDevice physicalDevice;
		std::shared_ptr<const std::vector<const char*>> deviceExtensions;
	};
}
