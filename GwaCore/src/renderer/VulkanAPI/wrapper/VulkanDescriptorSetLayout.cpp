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

			std::vector<VkDescriptorBindingFlags> bindingFlags(numberOfBindings, 0);
			for (uint32_t bindingIndex = 0; bindingIndex < numberOfBindings; bindingIndex++)
			{
				descriptorLayoutBindings[bindingIndex].descriptorType = static_cast<VkDescriptorType>(curDescriptorConfig.bindings[bindingIndex].type);
				descriptorLayoutBindings[bindingIndex].descriptorCount = curDescriptorConfig.bindings[bindingIndex].descriptorCount;
				descriptorLayoutBindings[bindingIndex].binding = curDescriptorConfig.bindings[bindingIndex].bindingSlot;
				descriptorLayoutBindings[bindingIndex].stageFlags = static_cast<VkShaderStageFlags>(curDescriptorConfig.bindings[bindingIndex].shaderStage);
				descriptorLayoutBindings[bindingIndex].pImmutableSamplers = nullptr;

				if (curDescriptorConfig.bindless)
				{
					bindingFlags[bindingIndex] =
						VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
						VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
				}
			}

			VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
			bindingFlagsCreateInfo.bindingCount = static_cast<uint32_t>(bindingFlags.size());
			bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();

			VkDescriptorSetLayoutCreateInfo layoutCreateInfo {};
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutCreateInfo.bindingCount = numberOfBindings;
			layoutCreateInfo.pBindings = descriptorLayoutBindings.data();
			layoutCreateInfo.flags = curDescriptorConfig.bindless? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT: 0;
			layoutCreateInfo.pNext = curDescriptorConfig.bindless ? &bindingFlagsCreateInfo : nullptr;

			VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutCreateInfo, nullptr, &descriptorSetLayouts[descriptorSetIndex]);
			assert(result == VK_SUCCESS);
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
