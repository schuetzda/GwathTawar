#pragma once
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <cassert>
#include <renderer/VulkanAPI/VulkanCommandBuffers.h>
namespace gwa::vulkanutil
{
	uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties);
	void copyBuffer(VkDevice logicalDevice, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	VkFormat chooseSupportedFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	std::vector<VulkanCommandBuffer> initCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, uint32_t commandBufferCount);
}