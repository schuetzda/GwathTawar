#include "VulkanCommandBuffers.h"
#include <stdexcept>
#include <cassert>

namespace gwa
{
	VulkanCommandBuffers::VulkanCommandBuffers(const VkDevice logicalDevice, const VkCommandPool commandPool, const int MAX_FRAME_DRAWS)
	{
		commandBuffers.resize(MAX_FRAME_DRAWS);

		VkCommandBufferAllocateInfo cbAllocInfo = {};
		cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbAllocInfo.commandPool = commandPool;
		cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Primary: Buffer you submit directly to queue. Can't be called by other buffers.

		cbAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, commandBuffers.data());
		assert(result == VK_SUCCESS);
	}
}
