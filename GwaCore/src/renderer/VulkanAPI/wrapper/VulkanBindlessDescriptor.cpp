#include "VulkanBindlessDescriptor.h"
#include <array>
#include <cassert>

namespace gwa::renderer
{
	VulkanBindlessDescriptor::VulkanBindlessDescriptor(VkDevice logicalDevice, uint32_t maxBindlessRessources, uint32_t bindlessTextureBinding, uint32_t MAX_FRAMES_IN_FLIGHT)
	{
		std::array<VkDescriptorPoolSize, 2> poolSizesBindless;
		poolSizesBindless[0] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxBindlessRessources };
		poolSizesBindless[1] = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxBindlessRessources };

		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
		poolCreateInfo.maxSets = maxBindlessRessources * static_cast<uint32_t>(poolSizesBindless.size());
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizesBindless.size());
		poolCreateInfo.pPoolSizes = poolSizesBindless.data();
		VkResult result = vkCreateDescriptorPool(logicalDevice, &poolCreateInfo, nullptr, &descriptorPool);
		assert(result == VK_SUCCESS);

		std::array<VkDescriptorSetLayoutBinding, 4> descriptorLayoutBindings;
		//Image sampler binding
		descriptorLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorLayoutBindings[0].descriptorCount = maxBindlessRessources;
		descriptorLayoutBindings[0].binding = bindlessTextureBinding;
		descriptorLayoutBindings[0].stageFlags = VK_SHADER_STAGE_ALL;
		descriptorLayoutBindings[0].pImmutableSamplers = nullptr;

		//Storage image binding
		descriptorLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorLayoutBindings[1].descriptorCount = maxBindlessRessources;
		descriptorLayoutBindings[1].binding = bindlessTextureBinding + 1;
		descriptorLayoutBindings[1].stageFlags = VK_SHADER_STAGE_ALL;
		descriptorLayoutBindings[1].pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = static_cast<uint32_t>(poolSizesBindless.size());
		layoutInfo.pBindings = descriptorLayoutBindings.data();
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

		const VkDescriptorBindingFlags bindlessFlag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | /*VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |*/ VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		std::array<VkDescriptorBindingFlags,4> bindingFlags;

		bindingFlags[0] = bindlessFlag;
		bindingFlags[1] = bindlessFlag;

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr };
		extendedInfo.bindingCount = static_cast<uint32_t>(poolSizesBindless.size());
		extendedInfo.pBindingFlags = bindingFlags.data();

		layoutInfo.pNext = &extendedInfo;

		result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &bindlessDescriptorSetLayout);
		assert(result == VK_SUCCESS);

		VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocateInfo.descriptorPool = descriptorPool;
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &bindlessDescriptorSetLayout;

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countAllocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
		const uint32_t maxBindings = maxBindlessRessources - 1;
		countAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		// This number is the max allocatable amount
		countAllocateInfo.pDescriptorCounts = &maxBindings;
		//alloc_info.pNext = &count_info;
		result = vkAllocateDescriptorSets(logicalDevice, &allocateInfo, descriptorSets.data());
		assert(result == VK_SUCCESS);
	}
	void VulkanBindlessDescriptor::addTextures(VkDevice logicalDevice, uint32_t MAX_FRAMES_IN_FLIGHT, const std::vector<VkImageView>& views, VkSampler textureSampler, uint32_t textureBinding)
	{
		for (uint32_t renderFrame = 0; renderFrame < MAX_FRAMES_IN_FLIGHT; renderFrame++)
		{
			std::vector<VkWriteDescriptorSet> bindlessDescriptorWrites;
			bindlessDescriptorWrites.resize(views.size());

			std::vector<VkDescriptorImageInfo> bindlessImageInfo;
			bindlessImageInfo.resize(views.size());

			for (uint32_t j = 0; j < views.size(); j++)
			{
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = views[j];
				imageInfo.sampler = textureSampler;
			
				VkWriteDescriptorSet& descriptorWrite = bindlessDescriptorWrites[j];
				descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrite.dstSet = descriptorSets[renderFrame];
				descriptorWrite.dstBinding = textureBinding;
				descriptorWrite.pImageInfo = &imageInfo;

			}
			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(bindlessDescriptorWrites.size()), bindlessDescriptorWrites.data(), 0, nullptr);
		}
	}
}

