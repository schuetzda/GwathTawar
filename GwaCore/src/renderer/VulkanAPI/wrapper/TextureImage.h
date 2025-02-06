#pragma once
#include <filesystem>
#include <vulkan/vulkan.h>
namespace gwa
{
	class TextureImage
	{
	public:
		TextureImage() = default;
		TextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
	private:
		void transitionImageLayout(VkImage image, VkFormat format, VkCommandPool commandPool, VkImageLayout oldLayout, VkImageLayout newLayout);
		
		VkDevice logicalDevice_{0};
	};
}
