#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanImage
	{
	public:
		VulkanImage(VkDevice& logicalDevice, VkPhysicalDevice physicalDevice, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
		void cleanup(VkDevice logicalDevice);
		VkImage& getImage()
		{
			return image;
		}
		VkDeviceMemory& getImageMemory()
		{
			return imageMemory;
		}
	private:
		static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties);

		VkImage image;
		VkDeviceMemory imageMemory;
	};
}
