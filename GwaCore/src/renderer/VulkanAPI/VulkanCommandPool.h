#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		void cleanup(VkDevice logicalDevice);
		
		VkCommandPool commandPool;
	};
}
