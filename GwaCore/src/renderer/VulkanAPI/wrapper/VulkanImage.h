#pragma once
#include <vulkan/vulkan_core.h>
#include "wrapper/VulkanDevice.h"
namespace gwa
{
	class VulkanImage
	{
	public:
		VulkanImage() = default;
		void init(VulkanDevice* device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
		void cleanup();

	private:
		VkImage image_;
		VkDeviceMemory imageMemory_;

		VkDevice logicalDevice_;
	};
}
