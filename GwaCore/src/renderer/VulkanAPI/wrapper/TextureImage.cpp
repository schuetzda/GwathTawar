#include "TextureImage.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include <renderer/VulkanAPI/VulkanCommandBuffers.h>

namespace gwa
{
    TextureImage::TextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) : logicalDevice_(logicalDevice)
    {
        int texWidth{ 0 }, texHeight{ 0 }, texChannels{ 0 };
        stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        assert(pixels);
        VulkanBuffer stagingBuffer = VulkanBuffer(logicalDevice_, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data;
        vkMapMemory(logicalDevice_, stagingBuffer.getBufferMemory(), 0, imageSize, 0, &data);
        memcpy(data, pixels, imageSize);
        vkUnmapMemory(logicalDevice_, stagingBuffer.getBufferMemory());

        stbi_image_free(pixels);

        VulkanImage image = VulkanImage(logicalDevice_, physicalDevice, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    void TextureImage::transitionImageLayout(VkImage image, VkFormat format, VkCommandPool commandPool, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VulkanCommandBuffer transferBuffer = VulkanCommandBuffer(logicalDevice_, commandPool);
        transferBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        vkCmdPipelineBarrier(
            *transferBuffer.getCommandBuffer(),
            0 /* TODO */, 0 /* TODO */,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        transferBuffer.endCommandBuffer();
    }
}
