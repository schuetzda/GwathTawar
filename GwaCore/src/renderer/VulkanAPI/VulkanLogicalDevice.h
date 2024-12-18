#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>
namespace gwa {
	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::shared_ptr<const std::vector<const char*>> deviceExtensions);
		void cleanup();

		VkDevice logicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentationQueue;
	private:	
		std::shared_ptr<const std::vector<const char*>> deviceExtensions;

	};
	
}
