#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapchain.h"
namespace gwa
{
	class VulkanUniformBuffers
	{
	public:
		VulkanUniformBuffers() = default;
		VulkanUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint64_t uniformBufferSize, const int imageCount_);
		void updateUniformBuffers(uint32_t imageIndex, uint64_t uniformBufferSize, const void* uniformBufferData);
		void cleanup();
		const std::vector<VkBuffer>& getUniformBuffers() const
		{
			return uniformBuffers_;
		}
	private:
		void createBuffer(VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) const;

		std::vector<VkDeviceMemory> uniformBufferMemory_{};
		int imageCount_{0};
		std::vector<VkBuffer> uniformBuffers_{};

		VkDevice logicalDevice_{};
	};
}
