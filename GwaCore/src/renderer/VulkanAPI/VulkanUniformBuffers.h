#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapchain.h"
namespace gwa
{
	class VulkanUniformBuffers
	{
	public:
		VulkanUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint64_t uniformBufferSize, const int IMAGE_COUNT);
		void createBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) const;
		void updateUniformBuffers(VkDevice logicalDevice, uint32_t imageIndex, uint64_t uniformBufferSize, const void* uniformBufferData);
		void cleanup(const VkDevice logicalDevice);

		std::vector<VkBuffer> uniformBuffers;
	private:
		std::vector<VkDeviceMemory> uniformBufferMemory;
		const int MAX_FRAMES_IN_FLIGHT;
	};
}
