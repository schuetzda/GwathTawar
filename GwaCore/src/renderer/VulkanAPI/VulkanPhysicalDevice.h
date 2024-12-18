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
		 VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, std::shared_ptr<const std::vector<const char*>> deviceExtensions);

		VkPhysicalDevice physicalDevice;
		std::shared_ptr<const std::vector<const char*>> deviceExtensions;
	private:

		std::vector<VkPhysicalDevice> getPhysicalDeviceList(const VkInstance& instance) const;
		int ratePhysicalDeviceSuitable(const VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface) const;
		bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice) const;
	};
}
