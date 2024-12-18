#include "VulkanCommandBuffers.h"
#include <stdexcept>
#include <cassert>

namespace gwa
{
	VulkanCommandBuffers::VulkanCommandBuffers(const VkDevice logicalDevice, const VkCommandPool commandPool, const int MAX_FRAME_DRAWS)
	{
		commandBuffers.resize(MAX_FRAME_DRAWS);

		VkCommandBufferAllocateInfo cbAllocInfo = {};
		cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbAllocInfo.commandPool = commandPool;
		cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Primary: Buffer you submit directly to queue. Can't be called by other buffers.

		cbAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, commandBuffers.data());
		assert(result == VK_SUCCESS);
	}

	void VulkanCommandBuffers::recordCommands(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer, VkPipeline pipeline, const uint32_t currentFrame)
	{
		VkCommandBufferBeginInfo bufferBeginInfo = {};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		// Start recording commands to command buffer
		VkResult result = vkBeginCommandBuffer(commandBuffers[currentFrame], &bufferBeginInfo);
		assert(result == VK_SUCCESS);

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

		vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
/*
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			VkBuffer vertexBuffer[] = { meshes[i].getVertexBuffer() };			// Buffers to bind
			VkDeviceSize offsets[] = { 0 };										// Offsets into buffers being bound
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer, offsets);	// Command to bind verte buffer

			// Bind index buffer with offset of 0 and using uint32 indices
			vkCmdBindIndexBuffer(commandBuffer, meshes[i].getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			/* NOT IN USE, for Dynamic UBO
			uint32_t dynamicOffset = static_cast<uint32_t>(modelUniformAlignment) * i;


			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				0, 1, &descriptorSets[i], 1, &dynamicOffset);*/
		/*
			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(Model),
				&meshes[i].getModel());

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				0, 1, &descriptorSets[currentFrame], 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, meshes[i].getIndexCount(), 1, 0, 0, 0);
		}
			*/

		vkCmdEndRenderPass(commandBuffers[currentFrame]);

		// Start recording commands to command buffer
		result = vkEndCommandBuffer(commandBuffers[currentFrame]);
		assert(result == VK_SUCCESS);
	}
}
