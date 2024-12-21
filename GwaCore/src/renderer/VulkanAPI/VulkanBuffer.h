#pragma once
#include <vulkan/vulkan.h>

namespace gwa
{
	class VulkanBuffer
	{
	public:
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;

		VulkanBuffer() = default;
		VulkanBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties);
		void cleanup(VkDevice logicalDevice);
	};
}
