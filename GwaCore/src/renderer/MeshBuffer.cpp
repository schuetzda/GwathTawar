#include "MeshBuffer.h"
#include <cstring>
#include "VulkanAPI/VulkanCommandBuffers.h"
namespace gwa
{

	MeshBuffer::MeshBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, std::span<Vertex> vertices, std::span<uint32_t> indices, VkQueue transferQueue,
		VkCommandPool transferCommandPool)
	{
		VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
		createMeshBuffer(vertexBuffer, logicalDevice, physicalDevice, transferQueue, transferCommandPool, bufferSize, vertices.data());

		bufferSize = sizeof(uint32_t) * indices.size();
		createMeshBuffer(indexBuffer, logicalDevice, physicalDevice,transferQueue, transferCommandPool, bufferSize, indices.data());
	}

	void MeshBuffer::createMeshBuffer(VulkanBuffer& buffer, VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue transferQueue,
		VkCommandPool transferCommandPool, VkDeviceSize bufferSize, const void* vertData)
	{
		VulkanBuffer stagingBuffer = VulkanBuffer(logicalDevice, physicalDevice, bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Map memory to staging buffer. CPU -> Staging Buffer
		void* data;
		vkMapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);	// Map the vertex buffer memory
		memcpy(data, vertData, bufferSize);
		vkUnmapMemory(logicalDevice, stagingBuffer.bufferMemory);							// Unmap vertexBuffer memory

		buffer = VulkanBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		copyBuffer(logicalDevice, transferQueue, transferCommandPool, stagingBuffer.buffer, vertexBuffer.buffer, bufferSize);

		stagingBuffer.cleanup(logicalDevice);
	}

	void MeshBuffer::copyBuffer(VkDevice logicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
	{
		//Create buffer
		VulkanCommandBuffers transferCommandBuffer = VulkanCommandBuffers(logicalDevice, transferCommandPool, 1);
		transferCommandBuffer.beginCommandBuffer(0);
		// Region of data to copy from and to
		VkBufferCopy bufferCopyRegion = {};
		bufferCopyRegion.srcOffset = 0;		// Where to we copy from the src
		bufferCopyRegion.dstOffset = 0;
		bufferCopyRegion.size = bufferSize;

		vkCmdCopyBuffer(transferCommandBuffer.commandBuffers[0], srcBuffer, dstBuffer, 1, &bufferCopyRegion);

		transferCommandBuffer.endCommandBuffer(0);

		// Queue submission information
		// TODO put somewhere else
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &transferCommandBuffer.commandBuffers[0];

		// For now since we only load one mesh there is no need for synchronization/ parallization
		vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(transferQueue);

		vkFreeCommandBuffers(logicalDevice, transferCommandPool, 1, &transferCommandBuffer.commandBuffers[0]);
	}

}
