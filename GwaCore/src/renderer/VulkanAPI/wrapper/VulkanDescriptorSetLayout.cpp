#include "VulkanDescriptorSetLayout.h"
#include <cassert>
namespace gwa::renderer
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice logicalDevice, const std::vector<DescriptorSetConfig>& descriptorSetsConfig)
	{
		const size_t numberDescriptorSets = descriptorSetsConfig.size();
		descriptorSetLayouts.resize(numberDescriptorSets);
		for (uint32_t descriptorSetIndex = 0; descriptorSetIndex < numberDescriptorSets; descriptorSetIndex++)
		{
			const uint32_t numberOfBindings = static_cast<uint32_t>(descriptorSetsConfig[descriptorSetIndex].bindings.size());
			std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings;
			descriptorLayoutBindings.resize(numberOfBindings);

			const DescriptorSetConfig curDescriptorConfig = descriptorSetsConfig[descriptorSetIndex];

			for (uint32_t bindingIndex = 0; bindingIndex < numberOfBindings; bindingIndex++)
			{
				descriptorLayoutBindings[bindingIndex].descriptorType = static_cast<VkDescriptorType>(curDescriptorConfig.bindings[bindingIndex].type);
				descriptorLayoutBindings[bindingIndex].descriptorCount = curDescriptorConfig.descriptorCount;
				descriptorLayoutBindings[bindingIndex].binding = curDescriptorConfig.bindings[bindingIndex].bindingSlot;
				descriptorLayoutBindings[bindingIndex].stageFlags = static_cast<VkShaderStageFlagBits>(curDescriptorConfig.bindings[bindingIndex].shaderStage);
				descriptorLayoutBindings[bindingIndex].pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			layoutInfo.bindingCount = numberOfBindings;
			layoutInfo.pBindings = descriptorLayoutBindings.data();
			layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
		}
	}

	void VulkanDescriptorSetLayout::cleanup(VkDevice logicalDevice)
	{
		for (VkDescriptorSetLayout descriptorSetLayout : descriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
		}
	}
}
