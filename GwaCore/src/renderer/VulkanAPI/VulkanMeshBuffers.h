#pragma once
#include <vulkan/vulkan.h>
#include <span>
#include <array>
#include "vkTypes.h"
#include <glm/glm.hpp>
#include "VulkanMeshBufferMemory.h"

namespace gwa::renderer
{
	class VulkanMeshBuffers
	{
		public:
		struct MeshBufferData {
			VkBuffer indexBuffer;
			std::array<VkBuffer, 3> vertexBuffers; // 0: Vertex, 1: Normal, 2: Texcoord
			uint32_t indexCount;
		};

		VulkanMeshBuffers() = default;

		uint32_t addBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VulkanMeshBufferMemory& bufferMemory, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texcoords, const std::vector<uint32_t>& indices, VkQueue transferQueue, VkCommandPool transferCommandPool);
		MeshBufferData getMeshBufferData(const uint32_t meshBufferIndex)
		{
			assert(meshBufferIndex + 1 <= meshBufferDataList_.size());
			return meshBufferDataList_[meshBufferIndex];
		}
		const std::vector<MeshBufferData>& getMeshBufferDataList() const
		{
			return meshBufferDataList_;
		}
		void cleanup(VkDevice logicalDevice);
	private:
		void copyAndSubmitBuffer(VkDevice logicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
		void createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties);
		void createMeshBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkBuffer& meshBuffer, VkDeviceMemory& meshBufferMemory, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, const void* vertData);

		std::vector<MeshBufferData> meshBufferDataList_;
	};
}
