#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace gwa::renderer
{
	struct VulkanMeshBufferMemory
	{
		VulkanMeshBufferMemory() = default;
		std::vector<VkDeviceMemory> vertexBufferMemoryList_{};
		std::vector<VkDeviceMemory> normalBufferMemoryList_{};
		std::vector<VkDeviceMemory> indexBufferMemoryList_{};
		std::vector<VkDeviceMemory> texcoordBufferMemoryList_{};

		void cleanup(VkDevice logicalDevice)
		{
			for (int i = 0; i < indexBufferMemoryList_.size(); ++i)
			{
				vkFreeMemory(logicalDevice, vertexBufferMemoryList_[i], nullptr);
				vkFreeMemory(logicalDevice, indexBufferMemoryList_[i], nullptr);
				vkFreeMemory(logicalDevice, normalBufferMemoryList_[i], nullptr);
				vkFreeMemory(logicalDevice, texcoordBufferMemoryList_[i], nullptr);
			}
		}
	};
}
