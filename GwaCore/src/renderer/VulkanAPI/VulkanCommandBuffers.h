#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers(const VkDevice logicalDevice, const VkCommandPool commandPool, const uint32_t commandBufferCount);
		void beginCommandBuffer(const uint32_t currentIndex);
		void endCommandBuffer(const uint32_t currentIndex);
		void recordCommands(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, VkPipeline pipeline, const uint32_t currentFrame);
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
