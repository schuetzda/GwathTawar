#include "VulkanMeshBuffers.h"
#include <cstring>
#include "VulkanCommandBuffers.h"
#include "wrapper/VulkanUtility.h"
namespace gwa::renderer
{

	uint32_t VulkanMeshBuffers::addBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VulkanMeshBufferMemory& bufferMemory, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texcoords, const std::vector<uint32_t>& indices, VkQueue transferQueue, VkCommandPool transferCommandPool)
	{
		MeshBufferData meshBufferData{};
		//Vertex Buffer
		bufferMemory.vertexBufferMemoryList_.emplace_back();
		const VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		
		createMeshBuffer(logicalDevice, physicalDevice, meshBufferData.vertexBuffers[0], bufferMemory.vertexBufferMemoryList_.back(), transferQueue, transferCommandPool, vertexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.data());

		bufferMemory.normalBufferMemoryList_.emplace_back();
		const VkDeviceSize normalBufferSize = sizeof(normals[0]) * normals.size();
		createMeshBuffer(logicalDevice, physicalDevice, meshBufferData.vertexBuffers[1], bufferMemory.normalBufferMemoryList_.back(), transferQueue, transferCommandPool, normalBufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, normals.data());

		bufferMemory.texcoordBufferMemoryList_.emplace_back();
		const VkDeviceSize texcoordBufferSize = sizeof(texcoords[0]) * texcoords.size();
		createMeshBuffer(logicalDevice, physicalDevice, meshBufferData.vertexBuffers[2], bufferMemory.texcoordBufferMemoryList_.back(), transferQueue, transferCommandPool, texcoordBufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texcoords.data());
		
		//Index Buffer
		bufferMemory.indexBufferMemoryList_.emplace_back();
		const VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

		meshBufferData.indexCount = static_cast<uint32_t>(indices.size());
		createMeshBuffer(logicalDevice, physicalDevice, meshBufferData.indexBuffer, bufferMemory.indexBufferMemoryList_.back(), transferQueue, transferCommandPool, indexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.data());
		
		meshBufferDataList_.push_back(meshBufferData);
		return static_cast<uint32_t>(meshBufferDataList_.size()-1);
	}

	void VulkanMeshBuffers::copyAndSubmitBuffer(VkDevice logicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
	{
		//Create buffer
		VulkanCommandBuffer transferCommandBuffer = VulkanCommandBuffer(logicalDevice, transferCommandPool);
		transferCommandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		// Region of data to copy from and to
		VkBufferCopy bufferCopyRegion = {};
		bufferCopyRegion.srcOffset = 0;		// Where to we copy from the src
		bufferCopyRegion.dstOffset = 0;
		bufferCopyRegion.size = bufferSize;

		vkCmdCopyBuffer(*transferCommandBuffer.getCommandBuffer(), srcBuffer, dstBuffer, 1, &bufferCopyRegion);

		transferCommandBuffer.endCommandBuffer();

		// Queue submission information
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = transferCommandBuffer.getCommandBuffer();

		// For now since we only load one mesh there is no need for synchronization/ parallization
		vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(transferQueue);

		vkFreeCommandBuffers(logicalDevice, transferCommandPool, 1, transferCommandBuffer.getCommandBuffer());
	}

	void VulkanMeshBuffers::createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties)
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
		memoryAllocInfo.memoryTypeIndex = vulkanutil::findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flag
			bufferProperties);							//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : CPU can interact with memory
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Allows placement of data straight into buffer after mapping
		result = vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, &bufferMemory);

		assert(result == VK_SUCCESS);

		// Allocate memory to given Vertex Buffer
		vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
	}

	void VulkanMeshBuffers::createMeshBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkBuffer& meshBuffer, VkDeviceMemory& meshBufferMemory, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, const void* vertData)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(logicalDevice, physicalDevice, stagingBuffer, stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Map memory to staging buffer. CPU -> Staging Buffer
		void* data;
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);	// Map the vertex buffer memory
		memcpy(data, vertData, bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);							// Unmap vertexBuffer memory

		createBuffer(logicalDevice, physicalDevice, meshBuffer, meshBufferMemory, bufferSize, usageFlags, propertyFlags);

		copyAndSubmitBuffer(logicalDevice, transferQueue, transferCommandPool, stagingBuffer, meshBuffer, bufferSize);
		
		vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
	}


	void VulkanMeshBuffers::cleanup(VkDevice logicalDevice)
	{
		for (int i = 0; i < meshBufferDataList_.size(); ++i)
		{
			vkDestroyBuffer(logicalDevice, meshBufferDataList_[i].vertexBuffers[0], nullptr);
			vkDestroyBuffer(logicalDevice, meshBufferDataList_[i].vertexBuffers[1], nullptr);
			vkDestroyBuffer(logicalDevice, meshBufferDataList_[i].vertexBuffers[2], nullptr);
			vkDestroyBuffer(logicalDevice, meshBufferDataList_[i].indexBuffer, nullptr);
		}
	}
}
