#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface);
		VkCommandPool& getCommandPool()
		{
			return commandPool;
		}
		void cleanup(VkDevice logicalDevice);
	private:
		VkCommandPool commandPool;
	};
}
