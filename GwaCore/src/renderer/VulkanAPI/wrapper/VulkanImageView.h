#pragma once
#include <vulkan/vulkan.h>
namespace gwa::renderer
{
	class VulkanImageView
	{
	public:
		VulkanImageView() = default;
		VulkanImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void recreateImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		void cleanup(VkDevice logicalDevice);
		VkImageView getImageView() const
		{
			return imageView;
		}
	private:
		void createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		VkImageView imageView;
	};
}
