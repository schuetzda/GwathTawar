#include "VulkanImage.h"
#include "VulkanUtility.h"
#include <stdexcept>
#include <cassert>
namespace gwa
{
	VulkanImage::VulkanImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, 
		VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags)
	{
		// CREATE IMAGE
		//Image Creation Info
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;					// Type of Image 1D, 2D, 3D
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = format;
		imageCreateInfo.tiling = tiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = useFlags;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image_);

		assert(result == VK_SUCCESS);
		// CREATE MEMORY FOR IMAGE

		// Get memory requiremts for a type of image
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice, image_, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = vulkanutil::findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, propFlags);

		result = vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, &imageMemory_);
		assert(result == VK_SUCCESS);

		// Connect memory to image
		vkBindImageMemory(logicalDevice, image_, imageMemory_, 0);
	}
	void VulkanImage::cleanup(VkDevice logicalDevice) 
	{
		vkDestroyImage(logicalDevice, image_, nullptr);
		vkFreeMemory(logicalDevice, imageMemory_, nullptr);

	}
}
