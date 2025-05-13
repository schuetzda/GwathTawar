#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanDevice.h"
namespace gwa
{
	class VulkanImage
	{
	public:
		VulkanImage() = default;
		VulkanImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, 
			VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
		void cleanup();
		 

		VkImage getImage() const
		{
			return image_;
		}
	private:
		void createImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
		VkImage image_{};
		VkDeviceMemory imageMemory_{};

		VkDevice logicalDevice_{};
	};
}
