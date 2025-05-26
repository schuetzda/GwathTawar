#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "vkTypes.h"
namespace gwa
{
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		explicit VulkanCommandBuffer(VkCommandBuffer commandBuffer);
		VulkanCommandBuffer(VkDevice logicalDevice, VkCommandPool commandPool);
		void beginCommandBuffer(VkCommandBufferUsageFlags flags);
		void beginRenderPass(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer);
		void bindPipeline(VkPipeline pipeline);
		void pipelineBarrier(VkImageMemoryBarrier barrier);
		void setViewport(const VkViewport& viewport);
		void setScissor(const VkRect2D& scissor);
		void endCommandBuffer();
		void bindVertexBuffer(const VkBuffer* vertexBuffers, uint32_t numBuffers, const VkDeviceSize* offsets) const;
		void bindIndexBuffer(VkBuffer indexBuffer) const;
		void pushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags flags, uint32_t pushSize, const void* pushValue);
		void bindDescriptorSet(uint32_t descriptorSetCount, const VkDescriptorSet* descriptorSets, VkPipelineLayout pipelineLayout);
		void drawIndexed(uint32_t indexCount);
		void endRenderPass();

		const VkCommandBuffer* getCommandBuffer()
		{
			return &commandBuffer_;
		}
	private:
		VkCommandBuffer commandBuffer_ = {};
	};
}
