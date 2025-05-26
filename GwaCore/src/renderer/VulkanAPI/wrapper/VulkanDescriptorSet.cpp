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
		std::vector<VkDescriptorSetLayout> setLayouts{};

		for (DescriptorSetConfig descriptorConfig : descriptorSetsConfig)
		{
			for (DescriptorBindingConfig bindingConfig : descriptorConfig.bindings)
			{
				if (descriptorConfig.bindless)
				{
					poolSizes.emplace_back(static_cast<VkDescriptorType>(bindingConfig.type), bindingConfig.descriptorCount);
					setLayouts.push_back(descriptorSetLayout[1]);
					maxSets++;
				}
				else
				{
					poolSizes.emplace_back(static_cast<VkDescriptorType>(bindingConfig.type), bindingConfig.descriptorCount * maxFramesInFlight);
					for (uint32_t i=0; i < maxFramesInFlight; i++)
					{
						setLayouts.push_back(descriptorSetLayout[0]);
					}
					maxSets += maxFramesInFlight;
				}
			}
		}

		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolCreateInfo.pPoolSizes = poolSizes.data();
		poolCreateInfo.maxSets = maxSets;
		poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;

		// Create Descriptor Pool
		VkResult result = vkCreateDescriptorPool(logicalDevice, &poolCreateInfo, nullptr, &descriptorPool);
		assert(result == VK_SUCCESS);


		std::vector<VkDescriptorSet> descriptorSets;
		descriptorSets.resize(maxSets);

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
				std::vector<VkDescriptorBufferInfo> bufferInfos;
				std::vector<VkDescriptorImageInfo> imageInfos;

				for (uint32_t bindingIndex = 0; bindingIndex < numberOfBindings; bindingIndex++)
				{
					descriptorWrites[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[bindingIndex].dstSet = descriptorSets[currentDescriptorSetIndex];
					descriptorWrites[bindingIndex].dstBinding = descriptorSetConfig.bindings[bindingIndex].bindingSlot;				// Binding to update matches with binding on layout/shader)
					descriptorWrites[bindingIndex].dstArrayElement = 0;		// Index in array to update
					descriptorWrites[bindingIndex].descriptorType = static_cast<VkDescriptorType>(descriptorSetConfig.bindings[bindingIndex].type);
					descriptorWrites[bindingIndex].descriptorCount = descriptorSetConfig.bindings[bindingIndex].descriptorCount;		// Amount to update
					for (uint32_t descriptorIndex = 0; descriptorIndex < descriptorWrites[bindingIndex].descriptorCount; descriptorIndex++)
					{
						switch (descriptorSetConfig.bindings[bindingIndex].type)
						{
						case DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER:
						{
							bufferInfos.emplace_back();
							bufferInfos.back().buffer = uniformBuffers[uniformBufferIndex];		// Buffer to get data from
							bufferInfos.back().offset = 0;						// position of start of data
							bufferInfos.back().range = dataSizes[uniformBufferIndex];

							descriptorWrites[bindingIndex].pBufferInfo = &bufferInfos[uniformBufferIndex];// Information of buffer data to bind
							uniformBufferIndex++;
							break;
						}
						case DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
						{
							imageInfos.emplace_back();
							imageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							imageInfos.back().imageView = textureImageView[textureViewIndex];
							imageInfos.back().sampler = textureSampler;

							descriptorWrites[bindingIndex].pImageInfo = imageInfos.data();
							textureViewIndex++;
							break;
						}
						default:
							std::cerr << "Vulkan Descriptor Set support for binding " << descriptorSetConfig.bindings[bindingIndex].bindingSlot;
						}
					}
				}
				vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
				descriptorSetsPerFrame.resize(maxFramesInFlight);
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
