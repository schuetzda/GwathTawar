#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, const int MAX_FRAME_DRAWS);
		void recordCommands(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, VkPipeline pipeline, const uint32_t currentFrame);
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
