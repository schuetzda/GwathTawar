#include "VulkanMeshBuffers.h"
#include <cstring>
#include "VulkanCommandBuffers.h"
#include "MemoryType.h"
namespace gwa
{

	int VulkanMeshBuffers::addBuffer(std::span<Vertex> vertices, std::span<uint32_t> indices, VkQueue transferQueue, VkCommandPool transferCommandPool)
	{
		meshBufferDataList.emplace_back();
		//Vertex Buffer
		vertexBuffers.emplace_back();
		vertexBufferMemoryList.emplace_back();
		const VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		
		createMeshBuffer(vertexBuffers.back(), vertexBufferMemoryList.back(), transferQueue, transferCommandPool, vertexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.data());

		//Index Buffer
		indexBufferMemoryList.emplace_back();
		const VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

		meshBufferDataList.back().indexCount = static_cast<uint32_t>(indices.size());
		createMeshBuffer(meshBufferDataList.back().indexBuffer, indexBufferMemoryList.back(), transferQueue, transferCommandPool, indexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.data());
		meshBufferDataList.back().vertexBuffer = vertexBuffers.back();

		return meshBufferDataList.size()-1;
	}

	void VulkanMeshBuffers::copyBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
	{
		//Create buffer
		VulkanCommandBuffers transferCommandBuffer = VulkanCommandBuffers(logicalDevice, transferCommandPool, 1);
		transferCommandBuffer.beginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
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

	void VulkanMeshBuffers::createBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties)
	{
		// Create Vertex Buffer
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = bufferUsage;							// Multiple types of buffers possible
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// Similar to swapchain images, can share vertex buffers

		VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);

		assert(result == VK_SUCCESS);

		// Get Buffer memory requirements
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

		// Allocate memory to buffer
		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = MemoryType::findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flag
			bufferProperties);							//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : CPU can interact with memory
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Allows placement of data straight into buffer after mapping
		result = vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, &bufferMemory);

		assert(result == VK_SUCCESS);

		// Allocate memory to given Vertex Buffer
		vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
	}

	void VulkanMeshBuffers::createMeshBuffer(VkBuffer& meshBuffer, VkDeviceMemory& meshBufferMemory, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, const void* vertData)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(stagingBuffer, stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Map memory to staging buffer. CPU -> Staging Buffer
		void* data;
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);	// Map the vertex buffer memory
		memcpy(data, vertData, bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);							// Unmap vertexBuffer memory

		createBuffer(meshBuffer, meshBufferMemory, bufferSize, usageFlags, propertyFlags);

		copyBuffer(transferQueue, transferCommandPool, stagingBuffer, meshBuffer, bufferSize);
		
		vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
	}


	void VulkanMeshBuffers::cleanup()
	{
		for (int i = 0; i < vertexBuffers.size(); ++i)
		{
			vkDestroyBuffer(logicalDevice, vertexBuffers[i], nullptr);
			vkDestroyBuffer(logicalDevice, meshBufferDataList[i].indexBuffer, nullptr);
		}
		for (int i = 0; i < indexBufferMemoryList.size(); ++i)
		{
			vkFreeMemory(logicalDevice, vertexBufferMemoryList[i], nullptr);
			vkFreeMemory(logicalDevice, indexBufferMemoryList[i], nullptr);
		}

	}
}
