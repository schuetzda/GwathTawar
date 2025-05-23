#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace gwa::renderer
{
	class VulkanBindlessDescriptor
	{
	public:
		VulkanBindlessDescriptor(VkDevice logicalDevice, uint32_t maxBindlessRessources, uint32_t bindlessTextureBinding, uint32_t MAX_FRAMES_IN_FLIGHT);
		void addTextures(VkDevice logicalDevice, uint32_t MAX_FRAMES_IN_FLIGHT, const std::vector<VkImageView>& views, VkSampler textureSampler, uint32_t textureBinding);
	private:
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout bindlessDescriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;
	};
}
