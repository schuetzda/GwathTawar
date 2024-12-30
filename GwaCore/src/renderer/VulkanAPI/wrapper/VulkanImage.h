#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanDevice.h"
namespace gwa
{
	class VulkanImage
	{
	public:
		VulkanImage() = default;
		VulkanImage(const VulkanDevice* const device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, 
			VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
		void cleanup();

		VkImage getImage() const
		{
			return image_;
		}
	private:
		VkImage image_{};
		VkDeviceMemory imageMemory_{};

		VkDevice logicalDevice_{};
	};
}
