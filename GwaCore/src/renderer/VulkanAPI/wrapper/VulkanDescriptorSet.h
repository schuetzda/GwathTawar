#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet() = default;
		VulkanDescriptorSet(VkDevice logicalDevice, VkDescriptorSetLayout descriptorSetLayout, const std::vector<VkBuffer>& uniformBuffers, 
			const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize);

		void cleanup();
		const std::vector<VkDescriptorSet>& getDescriptorSets() const
		{
			return descriptorSets_;
		}

		VkDescriptorPool getDescriptorPool() const
		{
			return descriptorPool_;
		}
	private:
		VkDescriptorPool descriptorPool_;
		std::vector<VkDescriptorSet> descriptorSets_;

		VkDevice logicalDevice_;
	};
}
