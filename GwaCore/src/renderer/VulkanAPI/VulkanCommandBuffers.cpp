#include "VulkanCommandBuffers.h"
#include <stdexcept>
#include <cassert>
#include <array>

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

	void VulkanCommandBuffer::beginRenderPass(uint32_t numberOfAttachments, VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, bool useDepthBuffer)
	{
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = extent;

		std::vector<VkClearValue> clearValues(numberOfAttachments);
		for (uint32_t i = 0; i < numberOfAttachments; ++i) {
			clearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f }; 	
		}

		if (useDepthBuffer)
		{
			clearValues.emplace_back();
			clearValues.back().depthStencil = { 1.0f, 0 };
		}

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();
		renderPassBeginInfo.framebuffer = framebuffer;

		vkCmdBeginRenderPass(commandBuffer_, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::bindPipeline(VkPipeline pipeline)
	{
		vkCmdBindPipeline(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void VulkanCommandBuffer::pipelineBarrier(const VkImageMemoryBarrier& barrier)
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

	void VulkanCommandBuffer::pushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags flags, uint32_t pushSize, const void* pushValue)
	{
		vkCmdPushConstants(
			commandBuffer_,
			pipelineLayout, flags,
			0,
			pushSize,
			pushValue);
	}

	void VulkanCommandBuffer::bindDescriptorSet(uint32_t descriptorSetCount, const VkDescriptorSet* descriptorSets, VkPipelineLayout pipelineLayout) 
	{
		vkCmdBindDescriptorSets(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, descriptorSetCount, descriptorSets, 0, nullptr);
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
