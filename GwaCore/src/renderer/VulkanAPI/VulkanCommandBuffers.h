#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers(VkDevice& logicalDevice, VkCommandPool& commandPool, const int MAX_FRAME_DRAWS);
		std::vector<VkCommandBuffer>& getCommandBuffers()
		{
			return commandBuffers;
		}
	private:
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
