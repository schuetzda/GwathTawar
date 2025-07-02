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
		void cleanup(VkDevice logicalDevice);
		 

		VkImage getImage() const
		{
			return image_;
		}
	private:
		VkImage image_{};
		VkDeviceMemory imageMemory_{};
	};
}
