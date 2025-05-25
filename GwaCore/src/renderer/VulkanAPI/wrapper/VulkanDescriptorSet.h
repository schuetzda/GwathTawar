#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "renderer/rendergraph/DescriptorSetConfigurator.h"
namespace gwa::renderer
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet() = default;
		VulkanDescriptorSet(VkDevice logicalDevice, VkDescriptorSetLayout descriptorSetLayout, const std::vector<VkBuffer>& uniformBuffers, 
			const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize);

		VulkanDescriptorSet(VkDevice logicalDevice, VkDescriptorSetLayout descriptorSetLayout, const std::vector<VkBuffer>& uniformBuffers, const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize, VkImageView textureImageView, VkSampler textureSampler);

		void cleanup();
		VkDescriptorSet createDescriptorSet(DescriptorSetConfig descriptorSetConfig);

		const std::vector<VkDescriptorSet>& getDescriptorSets() const
		{
			return descriptorSets;
		}

		VkDescriptorPool getDescriptorPool() const
		{
			return descriptorPool;
		}
	private:
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		VkDevice logicalDevice_;
	};
}
