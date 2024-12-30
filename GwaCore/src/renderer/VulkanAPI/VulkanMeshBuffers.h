#pragma once
#include <vulkan/vulkan.h>
#include <span>
#include "vkTypes.h"

namespace gwa
{
	class VulkanMeshBuffers
	{
		public:
		struct MeshBufferData {
			VkBuffer indexBuffer;
			VkBuffer vertexBuffer;
			uint32_t indexCount;
		};

		VulkanMeshBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) :logicalDevice(logicalDevice), physicalDevice(physicalDevice) 
		{
		
		}
		
		uint32_t addBuffer(std::span<Vertex> vertices, std::span<uint32_t> indices, VkQueue transferQueue, VkCommandPool transferCommandPool);
		MeshBufferData getMeshBufferData(const uint32_t meshBufferIndex)
		{
			assert(meshBufferIndex + 1 <= meshBufferDataList.size());
			return meshBufferDataList[meshBufferIndex];
		}
		void cleanup();
	private:
		void copyBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
		void createBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties);
		void createMeshBuffer(VkBuffer& meshBuffer, VkDeviceMemory& bufferMemory, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, const void* vertData);

		std::vector<MeshBufferData> meshBufferDataList;
		std::vector<VkDeviceMemory> vertexBufferMemoryList;
		std::vector<VkDeviceMemory> indexBufferMemoryList;
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
	};
}
