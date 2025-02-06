#include "VulkanUtility.h"

uint32_t gwa::vulkanutil::findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			// This memoryType is Valid so return index
			return i;
		}
	}
	assert(false);
	return 0;
}

void gwa::vulkanutil::copyBuffer(VkDevice logicalDevice, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VulkanCommandBuffer transferBuffer = VulkanCommandBuffer(logicalDevice, transferCommandPool);
	transferBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(*transferBuffer.getCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);

	transferBuffer.endCommandBuffer();
}

std::vector<gwa::VulkanCommandBuffer> gwa::vulkanutil::initCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, uint32_t commandBufferCount)
{
	std::vector<VkCommandBuffer> commandBuffers(commandBufferCount);

	VkCommandBufferAllocateInfo cbAllocInfo = {};
	cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAllocInfo.commandPool = commandPool;
	cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Primary: Buffer you submit directly to queue. Can't be called by other buffers.

	cbAllocInfo.commandBufferCount = commandBufferCount;

	VkResult result = vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, commandBuffers.data());
	assert(result == VK_SUCCESS);

	std::vector<VulkanCommandBuffer> vulkanCommandBuffers(commandBufferCount);

	for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; commandBufferIndex++)
	{
		vulkanCommandBuffers[commandBufferIndex] = VulkanCommandBuffer(commandBuffers[commandBufferIndex]);
	}
	return vulkanCommandBuffers;
}
