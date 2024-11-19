#include "VulkanCommandPool.h"
#include "QueueFamilyIndices.h"
#include <stdexcept>
namespace gwa
{
	VulkanCommandPool::VulkanCommandPool(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface)
	{
		QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::getQueueFamilyIndices(physicalDevice,surface);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

		VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create a command pool");
		}
	}
	void VulkanCommandPool::cleanup(VkDevice logicalDevice)
	{
		vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
	}
}
