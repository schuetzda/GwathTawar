#include "TextureImage.h"
#include "vulkan/vulkan.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "VulkanBuffer.h"

namespace gwa
{
    TextureImage::TextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) : logicalDevice_(logicalDevice)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }
        VulkanBuffer buffer(logicalDevice_, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    }
    void TextureImage::createTextureImage(std::filesystem::path path)
	{
        
        VulkanBuffer buffer = VulkanBuffer()
	}
}
