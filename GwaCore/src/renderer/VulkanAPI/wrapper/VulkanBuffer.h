#pragma once
#include <vulkan/vulkan.h>

namespace gwa
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer() = default;
		VulkanBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties);
		void cleanup();
	private:
		VkBuffer buffer_{};
		VkDeviceMemory bufferMemory_{};
		
		VkDevice logicalDevice_{};
	};
}
