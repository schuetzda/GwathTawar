#pragma once
#include <vulkan/vulkan.h>
#include "VulkanAPI/VulkanBuffer.h"
#include <span>
#include "VulkanAPI/vkTypes.h"

namespace gwa
{
	class MeshBuffer
	{
	public:
		MeshBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, std::span<Vertex> vertices, std::span<uint32_t> indices, VkQueue transferQueue, VkCommandPool transferCommandPool);

		void createMeshBuffer(VulkanBuffer& buffer, VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, const void* vertData);

		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;
	private:
		void copyBuffer(VkDevice logicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
	};
}
