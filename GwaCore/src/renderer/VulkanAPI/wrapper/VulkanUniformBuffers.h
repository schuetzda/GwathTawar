#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapchain.h"
#include <renderer/rendergraph/RenderGraphData.h>
namespace gwa::renderer
{
	class VulkanUniformBuffers
	{
	public:
		VulkanUniformBuffers() = default;
		VulkanUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint64_t uniformBufferSize, uint32_t maxFramesInFlight, ResourceAttachment uboResource);
		void updateUniformBuffers(VkDevice logicalDevice, uint32_t imageIndex, uint64_t uniformBufferSize, const void* uniformBufferData);
		void cleanup(VkDevice logicalDevice);

		const std::vector<VkBuffer>& getUniformBuffers() const
		{
			return uniformBuffers_;
		}
		const ResourceAttachment& getResource() const
		{
			return uboResource;
		}

	private:
		void createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) const;


		std::vector<VkDeviceMemory> uniformBufferMemory_{};
		std::vector<VkBuffer> uniformBuffers_{};
		ResourceAttachment uboResource;
	};
}
