#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa::renderer
{
	class VulkanImageViewCollection
	{
	public:
		VulkanImageViewCollection() = default;
		void addImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void addImageView(VkImageView imageView);
		void cleanup(VkDevice logicalDevice);
		void recreateImageView(uint32_t index, VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		 
		VkImageView getImageView(uint32_t index) const
		{
			if (index >= imageViews.size())
				return nullptr;
			return imageViews[index];
		}
		std::vector<VkImageView>& getImageViews() 
		{
			return imageViews;
		}
	private:
		void createImageView(uint32_t index, VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		std::vector<VkImageView> imageViews{};

	};
}
