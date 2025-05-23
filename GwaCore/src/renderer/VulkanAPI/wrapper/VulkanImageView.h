#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa::renderer
{
	class VulkanImageView
	{
	public:
		VulkanImageView() = default;
		void addImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void cleanup(VkDevice logicalDevice);
		 
		VkImageView getImageView(uint32_t index) const
		{
			return imageViews[index];
		}
		const std::vector<VkImageView>& getImageViews() const
		{
			return imageViews;
		}
	private:
		std::vector<VkImageView> imageViews{};

	};
}
