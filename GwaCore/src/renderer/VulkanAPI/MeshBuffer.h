#pragma once
#include <vulkan/vulkan.h>
#include "VulkanBuffer.h"
#include <span>
#include "vkTypes.h"

namespace gwa
{
	class MeshBuffer
	{
	public:

		MeshBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, VkQueue transferQueue, VkCommandPool transferCommandPool);

		void createMeshBuffer(VulkanBuffer& buffer, VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, const void* vertData);

		void cleanup(VkDevice logicalDevice);
		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;
		Model model; // TODO move
		uint32_t indexCount;
		uint32_t vertexCount;
	private:
		void copyBuffer(VkDevice logicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
		
	};
}
