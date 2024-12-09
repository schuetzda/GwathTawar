#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa
{
	class VulkanImageView
	{
	public:
		VulkanImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void cleanup(VkDevice logicalDevice);

		VkImageView& getImageView()
		{
			return imageView;
		}
	private:
		VkImageView imageView;
	};
}
