#include "VulkanCommandBuffers.h"
#include <stdexcept>
#include <cassert>

namespace gwa
{
	VulkanCommandBuffers::VulkanCommandBuffers(const VkDevice logicalDevice, const VkCommandPool commandPool, const uint32_t commandBufferCount)
	{
		commandBuffers.resize(commandBufferCount);

		VkCommandBufferAllocateInfo cbAllocInfo = {};
		cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbAllocInfo.commandPool = commandPool;
		cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Primary: Buffer you submit directly to queue. Can't be called by other buffers.

		cbAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, commandBuffers.data());
		assert(result == VK_SUCCESS);
	}

	void VulkanCommandBuffers::beginCommandBuffer(const uint32_t currentIndex, VkCommandBufferUsageFlags flags)
	{
		VkCommandBufferBeginInfo bufferBeginInfo = {};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bufferBeginInfo.flags = flags;
		// Start recording commands to command buffer
		VkResult result = vkBeginCommandBuffer(commandBuffers[currentIndex], &bufferBeginInfo);
		assert(result == VK_SUCCESS);
	}

	void VulkanCommandBuffers::beginRenderPass(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, const uint32_t currentIndex)
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

		vkCmdBeginRenderPass(commandBuffers[currentIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffers::bindPipeline(VkPipeline pipeline, const uint32_t currentIndex)
	{
		vkCmdBindPipeline(commandBuffers[currentIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void VulkanCommandBuffers::setViewport(const VkViewport& viewport, const uint32_t currentIndex)
	{
		vkCmdSetViewport(commandBuffers[currentIndex], 0, 1, &viewport);
	}

	void VulkanCommandBuffers::setScissor(const VkRect2D& scissor, const uint32_t currentIndex)
	{
		vkCmdSetScissor(commandBuffers[currentIndex], 0, 1, &scissor);
	}

	void VulkanCommandBuffers::endCommandBuffer(const uint32_t currentIndex)
	{
		vkEndCommandBuffer(commandBuffers[currentIndex]);
	}

	void VulkanCommandBuffers::bindVertexBuffer(const VkBuffer* vertexBuffers, const VkDeviceSize* offsets, const uint32_t currentIndex) const
	{
		vkCmdBindVertexBuffers(commandBuffers[currentIndex], 0, 1, vertexBuffers, offsets);
	}

	void VulkanCommandBuffers::bindIndexBuffer(VkBuffer indexBuffer, const uint32_t currrentIndex) const
	{
		vkCmdBindIndexBuffer(commandBuffers[currrentIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanCommandBuffers::pushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags flags, const uint32_t currentIndex, glm::mat4* model)
	{
		vkCmdPushConstants(
			commandBuffers[currentIndex],
			pipelineLayout, flags,
			0,
			sizeof(*model),
			model);
	}

	void VulkanCommandBuffers::bindDescriptorSet(VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, const int currentIndex) 
	{
		vkCmdBindDescriptorSets(commandBuffers[currentIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, 1, &descriptorSet, 0, nullptr);
	}

	void VulkanCommandBuffers::drawIndexed(uint32_t indexCount, const int currentIndex)
	{
		vkCmdDrawIndexed(commandBuffers[currentIndex], indexCount, 1, 0, 0, 0);
	}

	void VulkanCommandBuffers::endRenderPass(const int currentIndex)
	{
		vkCmdEndRenderPass(commandBuffers[currentIndex]);
	}

}
