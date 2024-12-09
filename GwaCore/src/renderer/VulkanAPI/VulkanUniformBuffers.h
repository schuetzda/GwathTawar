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
		const std::vector<VkBuffer>& getUniformBuffers() const
		{
			return uniformBuffers;
		}
		void cleanup(const VkDevice logicalDevice);
	private:
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBufferMemory;
		const int MAX_FRAMES_IN_FLIGHT;
	};
}
