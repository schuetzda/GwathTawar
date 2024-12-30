#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanImageView
	{
	public:
		VulkanImageView() = default;
		VulkanImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void cleanup();
		 
		VkImageView getImageView() const
		{
			return imageView_;
		}
	private:
		VkImageView imageView_{};

		VkDevice logicalDevice_{};
	};
}
