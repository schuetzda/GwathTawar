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
		void beginRenderPass(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, const uint32_t currentIndex);
		void bindPipeline(VkPipeline pipeline, const uint32_t currentIndex);
		void setViewport(const VkViewport& viewport, const uint32_t currentIndex);
		void setScissor(const VkRect2D& scissor, const uint32_t currentIndex);
		void endCommandBuffer(const uint32_t currentIndex);
		void bindVertexBuffer(const VkBuffer* vertexBuffers, const VkDeviceSize* offsets, const uint32_t currentIndex) const;
		void recordCommands(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, VkPipeline pipeline, const uint32_t currentFrame);
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
