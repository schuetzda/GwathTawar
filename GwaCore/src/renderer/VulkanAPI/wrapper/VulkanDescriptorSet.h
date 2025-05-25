#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <span>
#include "renderer/rendergraph/DescriptorSetConfigurator.h"
namespace gwa::renderer
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet() = default;

		VulkanDescriptorSet(VkDevice logicalDevice, std::span<const VkDescriptorSetLayout> descriptorSetLayout, std::span<const VkBuffer> uniformBuffers, uint32_t maxFramesInFlight, std::span<const uint64_t> dataSizes, std::span<const DescriptorSetConfig> descriptorSetsConfig, std::span<const VkImageView> textureImageView, VkSampler textureSampler);

		void cleanup(VkDevice logicalDevice);

		const std::vector<VkDescriptorSet>& getDescriptorSets(uint32_t currentFrame) const
		{
			return descriptorSetsPerFrame[currentFrame];
		}

		VkDescriptorPool getDescriptorPool() const
		{
			return descriptorPool;
		}
	private:
		VkDescriptorPool descriptorPool;
		std::vector<std::vector<VkDescriptorSet>> descriptorSetsPerFrame;
	};
}
