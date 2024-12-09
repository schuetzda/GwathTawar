#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, const int MAX_FRAME_DRAWS);
		const std::vector<VkCommandBuffer>& getCommandBuffers() const
		{
			return commandBuffers;
		}
	private:
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
