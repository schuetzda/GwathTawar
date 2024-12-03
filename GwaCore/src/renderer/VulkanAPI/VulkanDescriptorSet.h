#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VkDevice& logicalDevice, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkBuffer>& uniformBuffers, const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize);
	private:
		std::vector<VkDescriptorSet> descriptorSets;
	};
}
