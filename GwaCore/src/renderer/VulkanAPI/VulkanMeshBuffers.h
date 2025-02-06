#pragma once
#include <vulkan/vulkan.h>
#include <span>
#include "vkTypes.h"
#include <glm/glm.hpp>

namespace gwa
{
	class VulkanMeshBuffers
	{
		public:
		struct MeshBufferData {
			VkBuffer indexBuffer;
			VkBuffer vertexBuffer;
			VkBuffer normalBuffer;
			uint32_t indexCount;
		};

		VulkanMeshBuffers() = default;
		VulkanMeshBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) :logicalDevice_(logicalDevice), physicalDevice_(physicalDevice) {}

		uint32_t addBuffer(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<uint32_t>& indices, VkQueue transferQueue, VkCommandPool transferCommandPool);
		MeshBufferData getMeshBufferData(const uint32_t meshBufferIndex)
		{
			assert(meshBufferIndex + 1 <= meshBufferDataList_.size());
			return meshBufferDataList_[meshBufferIndex];
		}
		void cleanup();
	private:
		void copyAndSubmitBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
		void createBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties);
		void createMeshBuffer(VkBuffer& meshBuffer, VkDeviceMemory& bufferMemory, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, const void* vertData);

		std::vector<MeshBufferData> meshBufferDataList_;
		std::vector<VkDeviceMemory> vertexBufferMemoryList_;
		std::vector<VkDeviceMemory> normalBufferMemoryList_;
		std::vector<VkDeviceMemory> indexBufferMemoryList_;

		VkDevice logicalDevice_;
		VkPhysicalDevice physicalDevice_;
	};
}
