#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanImage
	{
	public:
		VulkanImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
		void cleanup(VkDevice logicalDevice);

		VkImage image;
	private:
		VkDeviceMemory imageMemory;
	};
}
