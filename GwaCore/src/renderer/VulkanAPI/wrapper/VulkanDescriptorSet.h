#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <span>
#include "renderer/rendergraph/DescriptorSetConfigurator.h"
#include <unordered_map>
#include "VulkanUniformBuffers.h"
#include "VulkanImageViewCollection.h"

namespace gwa::renderer
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet() = default;

		VulkanDescriptorSet(VkDevice logicalDevice, std::span<const VkDescriptorSetLayout> descriptorSetLayout, std::span<const VulkanUniformBuffers> uniformBuffers, uint32_t maxFramesInFlight, 
			std::span<const DescriptorSetConfig> descriptorSetsConfig, std::span<const VkImageView> textureImageView, VkSampler textureSampler, std::span<std::unordered_map<size_t, VkImageView>> framebufferImageViewsReference, 
			const std::vector<VulkanImageViewCollection>& imageViewCollections);



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
		void updateDescriptorSet(uint32_t currentFrame, VkDevice logicalDevice, const DescriptorSetConfig& descriptorSetConfig, VkDescriptorSet descriptorSet, std::span<const VulkanUniformBuffers> uniformBuffers, std::span<const VkImageView> textureImageView, VkSampler textureSampler, const std::unordered_map<size_t, VkImageView>& framebufferImageViewsReference, const VulkanImageViewCollection& imageViews);

		VkDescriptorPool descriptorPool{};
		std::vector<std::vector<VkDescriptorSet>> descriptorSetsPerFrame{};
	};
}
