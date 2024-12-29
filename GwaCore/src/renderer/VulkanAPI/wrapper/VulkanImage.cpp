#include "VulkanImage.h"
#include "MemoryType.h"
#include <stdexcept>
#include <cassert>
namespace gwa
{
	void VulkanImage::init(VulkanDevice* device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags)
	{
		logicalDevice_ = device->getLogicalDevice();
		// CREATE IMAGE
		//Image Creation Info
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;					// Type of Image 1D, 2D, 3D
		imageCreateInfo.extent.width = extent.width;
		imageCreateInfo.extent.height = extent.height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = format;
		imageCreateInfo.tiling = tiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = useFlags;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateImage(logicalDevice_, &imageCreateInfo, nullptr, &image_);

		assert(result == VK_SUCCESS);
		// CREATE MEMORY FOR IMAGE

		// Get memory requiremts for a type of image
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice_, image_, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = MemoryType::findMemoryTypeIndex(device->getPhysicalDevice(), memoryRequirements.memoryTypeBits, propFlags);

		result = vkAllocateMemory(logicalDevice_, &memoryAllocInfo, nullptr, &imageMemory_);
		assert(result == VK_SUCCESS);

		// Connect memory to image
		vkBindImageMemory(logicalDevice_, image_, imageMemory_, 0);
	}
	void VulkanImage::cleanup()
	{
		vkDestroyImage(logicalDevice_, image_, nullptr);
		vkFreeMemory(logicalDevice_, imageMemory_, nullptr);

	}
}