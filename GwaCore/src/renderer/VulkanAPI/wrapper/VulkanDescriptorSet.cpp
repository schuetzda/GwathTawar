#include "VulkanDescriptorSet.h"
#include <stdexcept>
#include <cassert>
#include <array>
#include <span>
#include <iostream>
namespace gwa::renderer
{
	VulkanDescriptorSet::VulkanDescriptorSet(VkDevice logicalDevice, std::span<const VkDescriptorSetLayout> descriptorSetLayout, std::span<const VkBuffer> uniformBuffers,
		uint32_t maxFramesInFlight, std::span<const uint64_t> dataSizes, std::span<const DescriptorSetConfig> descriptorSetsConfig, std::span<const VkImageView> textureImageView, VkSampler textureSampler)
	{
		//---DescriptorPool---
		uint32_t bindingIndex = 0;
		std::vector<VkDescriptorPoolSize> poolSizes{};
		uint32_t maxSets = 0;

		for (DescriptorSetConfig descriptorConfig : descriptorSetsConfig)
		{
			for (DescriptorBindingConfig bindingConfig : descriptorConfig.bindings)
			{
				if (descriptorConfig.bindless)
				{
					poolSizes.emplace_back(static_cast<VkDescriptorType>(bindingConfig.type), bindingConfig.descriptorCount);
					maxSets++;
				}
				else
				{
					poolSizes.emplace_back(static_cast<VkDescriptorType>(bindingConfig.type), bindingConfig.descriptorCount * maxFramesInFlight);
					maxSets += maxFramesInFlight;
				}
			}
		}

		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolCreateInfo.pPoolSizes = poolSizes.data();
		poolCreateInfo.maxSets = maxSets;

		// Create Descriptor Pool
		VkResult result = vkCreateDescriptorPool(logicalDevice, &poolCreateInfo, nullptr, &descriptorPool);
		assert(result == VK_SUCCESS);


		std::vector<VkDescriptorSet> descriptorSets;
		descriptorSets.resize(maxSets);

		//TODO set layout dynamically
		std::vector<VkDescriptorSetLayout> setLayouts(maxSets);
		for (size_t i = 0; i < maxSets; i++)
		{
			if (descriptorSetsConfig[i].bindless)
			{
				setLayouts[i] = descriptorSetLayout[1];
			}
			else
			{
				setLayouts[i] = descriptorSetLayout[0];
			}
		}

		VkDescriptorSetAllocateInfo setAllocInfo = {};
		setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocInfo.descriptorPool = descriptorPool;
		setAllocInfo.descriptorSetCount = maxSets;
		setAllocInfo.pSetLayouts = setLayouts.data();

		result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, descriptorSets.data());
		assert(result == VK_SUCCESS);

		uint32_t currentDescriptorSetIndex = 0;
		for (const DescriptorSetConfig& descriptorSetConfig: descriptorSetsConfig)
		{
			uint32_t numberOfFrames = 1;
			if (!descriptorSetConfig.bindless)
			{
				numberOfFrames = maxFramesInFlight;
			}

			for (uint32_t i = 0; i < numberOfFrames; i++)
			{
				std::vector<VkWriteDescriptorSet> descriptorWrites{};
				const uint32_t numberOfBindings = descriptorSetConfig.bindings.size();
				descriptorWrites.resize(numberOfBindings);
				uint32_t uniformBufferIndex = 0;
				uint32_t textureViewIndex = 0;
				for (uint32_t bindingIndex = 0; bindingIndex < numberOfBindings; bindingIndex++)
				{
					descriptorWrites[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[bindingIndex].dstSet = descriptorSets[currentDescriptorSetIndex];
					descriptorWrites[bindingIndex].dstBinding = descriptorSetConfig.bindings[bindingIndex].bindingSlot;				// Binding to update matches with binding on layout/shader)
					descriptorWrites[bindingIndex].dstArrayElement = 0;		// Index in array to update
					descriptorWrites[bindingIndex].descriptorType = static_cast<VkDescriptorType>(descriptorSetConfig.bindings[bindingIndex].type);
					descriptorWrites[bindingIndex].descriptorCount = descriptorSetConfig.bindings[bindingIndex].descriptorCount;		// Amount to update

					switch (descriptorSetConfig.bindings[bindingIndex].type)
					{
					case DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					{
						VkDescriptorBufferInfo vpBufferInfo = {};
						vpBufferInfo.buffer = uniformBuffers[uniformBufferIndex];		// Buffer to get data from
						vpBufferInfo.offset = 0;						// position of start of data
						vpBufferInfo.range = dataSizes[uniformBufferIndex];

						descriptorWrites[bindingIndex].pBufferInfo = &vpBufferInfo;// Information of buffer data to bind
						uniformBufferIndex++;
						break;
					}
					case DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					{
						VkDescriptorImageInfo imageInfo{};
						imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						imageInfo.imageView = textureImageView[textureViewIndex];
						imageInfo.sampler = textureSampler;

						descriptorWrites[bindingIndex].pImageInfo = &imageInfo;
						textureViewIndex++;
						break;
					}
					default:
						std::cerr << "Vulkan Descriptor Set support for binding " << descriptorSetConfig.bindings[bindingIndex].bindingSlot;
					}
				}
				vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

				if (descriptorSetConfig.bindless) {
					// Same set for every frame
					for (uint32_t frame = 0; frame < maxFramesInFlight; frame++) {
						descriptorSetsPerFrame[frame].push_back(descriptorSets[currentDescriptorSetIndex]);
					}
				}
				else {
					// One set per frame
					descriptorSetsPerFrame[i].push_back(descriptorSets[currentDescriptorSetIndex]);
				}

				currentDescriptorSetIndex++;
			}
		}
	}

	void VulkanDescriptorSet::cleanup(VkDevice logicalDevice)
	{
		vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	}
}
