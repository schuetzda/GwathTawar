#include "VulkanImageViewCollection.h"
#include <stdexcept>
#include <cassert>
namespace gwa::renderer
{
	void VulkanImageViewCollection::addImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
		imageViews.emplace_back();
		createImageView(static_cast<uint32_t>(imageViews.size() - 1), logicalDevice, image, format, aspectFlags);
	}
	void VulkanImageViewCollection::addImageView(VkImageView imageView)
	{
		imageViews.push_back(imageView);
	}
	void VulkanImageViewCollection::cleanup(VkDevice logicalDevice) 
	{
		for (VkImageView imageView : imageViews)
		{
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}
		imageViews.clear();
	}
	void VulkanImageViewCollection::recreateImageView(uint32_t index, VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		createImageView(index, logicalDevice, image, format, aspectFlags);
	}
	void VulkanImageViewCollection::createImageView(uint32_t index, VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = image;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = format;
		// Allows remapping of rgba compononents to other rgba values
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//Subresources allow the view to view only a part of an image
		viewCreateInfo.subresourceRange.aspectMask = aspectFlags;		//Wich aspect of image to view (e.g. COLOR_BIT for view color)
		viewCreateInfo.subresourceRange.baseMipLevel = 0;				// Start mipmap level to view from
		viewCreateInfo.subresourceRange.levelCount = 1;					// Number of mipmap levels to view
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;				// Start array level to view from
		viewCreateInfo.subresourceRange.layerCount = 1;					// number of array levels to view

		//Create image view and return it
		VkResult result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &imageViews[index]);
		assert(result == VK_SUCCESS);
	}
}
