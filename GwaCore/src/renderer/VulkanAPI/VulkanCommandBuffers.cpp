#include "VulkanCommandBuffers.h"
#include <stdexcept>

namespace gwa
{
	VulkanCommandBuffers::VulkanCommandBuffers(VkDevice& logicalDevice, VkCommandPool& commandPool, const int MAX_FRAME_DRAWS)
	{
		commandBuffers.resize(MAX_FRAME_DRAWS);

		VkCommandBufferAllocateInfo cbAllocInfo = {};
		cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbAllocInfo.commandPool = commandPool;
		cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Primary: Buffer you submit directly to queue. Can't be called by other buffers.

		cbAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, commandBuffers.data());
		if (result != VK_SUCCESS)
		{
			throw new std::runtime_error("Failed to allocate Command Buffers!");
		}
	}
}
