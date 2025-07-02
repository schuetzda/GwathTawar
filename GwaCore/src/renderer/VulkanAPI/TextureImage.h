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
		void cleanup(VkDevice logicalDevice);
	private:
		void transitionImageLayout(VkDevice logicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void copyBufferToImage(VkDevice logicalDevice, VkBuffer buffer, VkImage image, VkQueue graphicsQueue, VkCommandPool commandPool, uint32_t width, uint32_t height) const;
		void createImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, const Texture& texture, VkCommandPool commandPool);

		VulkanImage textureImage_{};
	};
}
