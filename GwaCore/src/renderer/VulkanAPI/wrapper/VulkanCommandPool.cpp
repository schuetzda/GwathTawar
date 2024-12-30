#include "VulkanCommandPool.h"
#include "QueueFamilyIndices.h"
#include <stdexcept>
#include <cassert>

namespace gwa
{
	VulkanCommandPool::VulkanCommandPool(const VulkanDevice* device):logicalDevice_(device->getLogicalDevice())
	{
		QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::getQueueFamilyIndices(device->getPhysicalDevice(), device->getSurface());

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

		VkResult result = vkCreateCommandPool(device->getLogicalDevice(), &poolInfo, nullptr, &commandPool_);
		assert(result == VK_SUCCESS);
	}
	void VulkanCommandPool::cleanup()
	{
		vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);
	}
}
