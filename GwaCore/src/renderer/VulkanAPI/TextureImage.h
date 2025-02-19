#pragma once
#include <filesystem>
#include <vulkan/vulkan.h>
#include "wrapper/VulkanImage.h"
#include "ecs/components/RenderObjects.h"
namespace gwa
{
	class TextureImage
	{
	public:
		TextureImage() = default;
		TextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, const Texture& texture, VkCommandPool commandPool);
		VulkanImage getTextureImage() const
		{
			return textureImage_;
		}
	private:
		void transitionImageLayout(VkDevice logicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkDevice logicalDevice, VkBuffer buffer, VkImage image, VkQueue graphicsQueue, VkCommandPool commandPool, uint32_t width, uint32_t height);

		VulkanImage textureImage_{};
	};
}
