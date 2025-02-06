#include "VulkanCommandBuffers.h"
#include <stdexcept>
#include <cassert>

namespace gwa
{
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer)
	{
		commandBuffer_ = commandBuffer;
	}
	VulkanCommandBuffer::VulkanCommandBuffer(VkDevice logicalDevice, VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo cbAllocInfo = {};
		cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbAllocInfo.commandPool = commandPool;
		cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Primary: Buffer you submit directly to queue. Can't be called by other buffers.

		cbAllocInfo.commandBufferCount = 1;

		VkResult result = vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, &commandBuffer_);
		assert(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::beginCommandBuffer(VkCommandBufferUsageFlags flags)
	{
		VkCommandBufferBeginInfo bufferBeginInfo = {};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bufferBeginInfo.flags = flags;
		// Start recording commands to command buffer
		VkResult result = vkBeginCommandBuffer(commandBuffer_, &bufferBeginInfo);
		assert(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::beginRenderPass(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer)
	{
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = extent;

		const uint32_t clearValuesSize = 2;
		VkClearValue clearValues[2] = { VkClearValue(),VkClearValue() };
		clearValues[0].color = { .6f, .65f, .4f, 1.f };
		clearValues[1].depthStencil.depth = 1.f;

		renderPassBeginInfo.pClearValues = clearValues;
		renderPassBeginInfo.clearValueCount = clearValuesSize;
		renderPassBeginInfo.framebuffer = framebuffer;

		vkCmdBeginRenderPass(commandBuffer_, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::bindPipeline(VkPipeline pipeline)
	{
		vkCmdBindPipeline(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void VulkanCommandBuffer::pipelineBarrier(VkImageMemoryBarrier barrier)
	{
		vkCmdPipelineBarrier(
			commandBuffer_,
			0 /* TODO */, 0 /* TODO */,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	void VulkanCommandBuffer::setViewport(const VkViewport& viewport)
	{
		vkCmdSetViewport(commandBuffer_, 0, 1, &viewport);
	}

	void VulkanCommandBuffer::setScissor(const VkRect2D& scissor)
	{
		vkCmdSetScissor(commandBuffer_, 0, 1, &scissor);
	}

	void VulkanCommandBuffer::endCommandBuffer()
	{
		vkEndCommandBuffer(commandBuffer_);
	}

	void VulkanCommandBuffer::bindVertexBuffer(const VkBuffer* vertexBuffers, uint32_t numBuffers, const VkDeviceSize* offsets) const
	{
		vkCmdBindVertexBuffers(commandBuffer_, 0, numBuffers, vertexBuffers, offsets);
	}

	void VulkanCommandBuffer::bindIndexBuffer(VkBuffer indexBuffer) const
	{
		vkCmdBindIndexBuffer(commandBuffer_, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanCommandBuffer::pushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags flags, glm::mat4 const * const model)
	{
		vkCmdPushConstants(
			commandBuffer_,
			pipelineLayout, flags,
			0,
			sizeof(*model),
			model);
	}

	void VulkanCommandBuffer::bindDescriptorSet(VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout) 
	{
		vkCmdBindDescriptorSets(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, 1, &descriptorSet, 0, nullptr);
	}

	void VulkanCommandBuffer::drawIndexed(uint32_t indexCount)
	{
		vkCmdDrawIndexed(commandBuffer_, indexCount, 1, 0, 0, 0);
	}

	void VulkanCommandBuffer::endRenderPass()
	{
		vkCmdEndRenderPass(commandBuffer_);
	}

	
}
