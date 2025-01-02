#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "vkTypes.h"
namespace gwa
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers() = default;
		VulkanCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, const uint32_t commandBufferCount);
		void beginCommandBuffer(const uint32_t currentIndex, VkCommandBufferUsageFlags flags);
		void beginRenderPass(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, const uint32_t currentIndex);
		void bindPipeline(VkPipeline pipeline, const uint32_t currentIndex);
		void setViewport(const VkViewport& viewport, const uint32_t currentIndex);
		void setScissor(const VkRect2D& scissor, const uint32_t currentIndex);
		void endCommandBuffer(const uint32_t currentIndex);
		void bindVertexBuffer(const VkBuffer* vertexBuffers, const VkDeviceSize* offsets, const uint32_t currentIndex) const;
		void bindIndexBuffer(VkBuffer indexBuffer, const uint32_t currrentIndex) const;
		void pushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags flags, const uint32_t currentIndex, glm::mat4* model);
		void bindDescriptorSet(VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, const int currentIndex);
		void drawIndexed(uint32_t indexCount, const int currentIndex);
		void endRenderPass(const int currentIndex);

		const VkCommandBuffer* getCommandBuffer(const int currentIndex)
		{
			return &commandBuffers_[currentIndex];
		}
	private:
		std::vector<VkCommandBuffer> commandBuffers_ = {};
	};
}
