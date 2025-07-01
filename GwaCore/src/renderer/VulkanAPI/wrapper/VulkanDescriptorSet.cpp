#include "VulkanDescriptorSet.h"
#include <stdexcept>
#include <cassert>
#include <array>
#include <span>
#include <iostream>
namespace gwa::renderer
{
	VulkanDescriptorSet::VulkanDescriptorSet(VkDevice logicalDevice, std::span<const VkDescriptorSetLayout> descriptorSetLayout, std::span<const VulkanUniformBuffers> uniformBuffers,
		uint32_t maxFramesInFlight, std::span<const DescriptorSetConfig> descriptorSetsConfig, std::span<const VkImageView> textureImageView, VkSampler textureSampler, VkSampler framebufferSampler,
		std::span<std::unordered_map<size_t, VkImageView>> framebufferImageViewsReference, const std::vector<VulkanImageViewCollection>& imageViewCollections)
	{
		attachmentReferenceBindings.resize(maxFramesInFlight);
		std::vector<VkDescriptorPoolSize> poolSizes{};
		uint32_t maxSets = 0;
		std::vector<VkDescriptorSetLayout> setLayouts{};

		//Create the descriptor pool and set the layouts per set. Per bindless descriptor one set is needed otherwise a descriptor set per frame in flight.
		for (uint32_t descriptorIndex = 0; descriptorIndex < descriptorSetsConfig.size(); descriptorIndex++)
		{
			const DescriptorSetConfig& descriptorConfig = descriptorSetsConfig[descriptorIndex];
			uint32_t setsToAdd = descriptorConfig.bindless ? 1 : maxFramesInFlight;
			VkDescriptorSetLayout layout = descriptorConfig.bindless ? descriptorSetLayout[1] : descriptorSetLayout[0];

			setLayouts.insert(setLayouts.end(), setsToAdd, layout);
			maxSets += setsToAdd;

			for (const DescriptorBindingConfig bindingConfig : descriptorConfig.bindings)
			{
				uint32_t descriptorCount = descriptorConfig.bindless
					? bindingConfig.maxDescriptorCount
					: bindingConfig.descriptorCount * maxFramesInFlight;
				poolSizes.emplace_back(static_cast<VkDescriptorType>(bindingConfig.type), descriptorCount);
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
		uint32_t frameDescriptorSetIndex = 0;
		descriptorSetsPerFrame.resize(maxFramesInFlight);
		for (const DescriptorSetConfig& descriptorSetConfig: descriptorSetsConfig)
		{
			uint32_t framesToProcess = descriptorSetConfig.bindless ? 1 : maxFramesInFlight;

			for (uint32_t i = 0; i < framesToProcess; i++)
			{
				updateDescriptorSet(i, frameDescriptorSetIndex, logicalDevice, descriptorSetConfig, descriptorSets[currentDescriptorSetIndex],
					uniformBuffers, textureImageView, textureSampler, framebufferSampler, framebufferImageViewsReference[i], imageViewCollections[i]);

				if (descriptorSetConfig.bindless) {
					// If the descriptorSet is bindless we don't have to create a new descriptor set per frame in flight. We can just reuse the same for every frame
					for (uint32_t frame = 0; frame < maxFramesInFlight; frame++) {
						descriptorSetsPerFrame[frame].push_back(descriptorSets[currentDescriptorSetIndex]);
					}
				}
				else {
					// If we are not bindless we have to update every descriptor set per frame
					descriptorSetsPerFrame[i].push_back(descriptorSets[currentDescriptorSetIndex]);
				}

				currentDescriptorSetIndex++;
			}
			frameDescriptorSetIndex++;
		}
	}

	void VulkanDescriptorSet::updateDescriptorSet(uint32_t currentFrame, uint32_t descriptorIndex, VkDevice logicalDevice, const DescriptorSetConfig& descriptorSetConfig, VkDescriptorSet descriptorSet, std::span<const VulkanUniformBuffers> uniformBuffers, 
		std::span<const VkImageView> textureImageView, VkSampler textureSampler, VkSampler framebufferSampler, const std::unordered_map<size_t, VkImageView>& framebufferImageViewsReference, const VulkanImageViewCollection& imageViews)
	{

		std::vector<VkWriteDescriptorSet> descriptorWrites{};
		const size_t numberOfBindings = descriptorSetConfig.bindings.size();
		descriptorWrites.resize(numberOfBindings);
		uint32_t uniformBufferIndex = 0;
		uint32_t textureViewIndex = 0;
		uint32_t imageReferenceIndex = 0;

		std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
		std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;

		//Reserving is crucial otherwise moving the data when resizing the vector will invalidate the pointer at descriptorWrites pBufferInfo
		bufferInfos.reserve(numberOfBindings);
		imageInfos.reserve(numberOfBindings);
		
		for (uint32_t bindingIndex = 0; bindingIndex < numberOfBindings; bindingIndex++)
		{
			const DescriptorBindingConfig& binding = descriptorSetConfig.bindings[bindingIndex];

			descriptorWrites[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[bindingIndex].dstSet = descriptorSet;
			descriptorWrites[bindingIndex].dstBinding = binding.bindingSlot;				// Binding to update matches with binding on layout/shader)
			descriptorWrites[bindingIndex].dstArrayElement = 0;		// Index in array to update
			descriptorWrites[bindingIndex].descriptorType = static_cast<VkDescriptorType>(binding.type);
			descriptorWrites[bindingIndex].descriptorCount = binding.descriptorCount;		// Amount to update

			switch (binding.type)
			{
			case DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			{
				bufferInfos.emplace_back();
				bufferInfos[uniformBufferIndex].resize(binding.descriptorCount);
				for (uint32_t descriptorIndex = 0; descriptorIndex < binding.descriptorCount; descriptorIndex++)
				{
					bufferInfos[uniformBufferIndex][descriptorIndex].buffer = uniformBuffers[uniformBufferIndex].getUniformBuffers()[currentFrame];		// Buffer to get data from
					bufferInfos[uniformBufferIndex][descriptorIndex].offset = 0;						// position of start of data
					bufferInfos[uniformBufferIndex][descriptorIndex].range = uniformBuffers[uniformBufferIndex].getResource().dataInfo.size;

				}
				descriptorWrites[bindingIndex].pBufferInfo = bufferInfos[uniformBufferIndex].data();// Information of buffer data to bind
				uniformBufferIndex++;
				break;
			}
			case DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			{
				imageInfos.emplace_back();
				const bool isAttachmentReference = binding.isAttachmentReference;
				imageInfos[imageReferenceIndex].resize(binding.descriptorCount);
				for (uint32_t descriptorIndex = 0; descriptorIndex < descriptorWrites[bindingIndex].descriptorCount; descriptorIndex++)
				{
					imageInfos[imageReferenceIndex][descriptorIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfos[imageReferenceIndex][descriptorIndex].imageView = isAttachmentReference ? framebufferImageViewsReference.at(binding.inputAttachmentHandle) : textureImageView[textureViewIndex++];
					imageInfos[imageReferenceIndex][descriptorIndex].sampler = isAttachmentReference ? framebufferSampler : textureSampler;
				}
				descriptorWrites[bindingIndex].pImageInfo = imageInfos[imageReferenceIndex].data();
				imageReferenceIndex++;
				if (isAttachmentReference)
				{
					attachmentReferenceBindings[currentFrame].push_back(std::pair<DescriptorBindingConfig, uint32_t>(binding, descriptorIndex));
				}
				break;
			}
			default:
				std::cerr << "Vulkan Descriptor Set support for binding " << descriptorSetConfig.bindings[bindingIndex].bindingSlot << " not implemented yet.";
			}
		}

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void VulkanDescriptorSet::cleanup(VkDevice logicalDevice)
	{
		vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	}
	void VulkanDescriptorSet::updateAttachmentReferences(VkDevice logicalDevice, std::span<std::unordered_map<size_t, VkImageView>> framebufferImageViewsReference, VkSampler framebufferSampler)
	{
		for (uint32_t frameIndex = 0; frameIndex < attachmentReferenceBindings.size(); frameIndex++)
		{
			const size_t numberOfBindings = attachmentReferenceBindings[frameIndex].size();
			std::vector<VkWriteDescriptorSet> descriptorWrites{};

			descriptorWrites.resize(numberOfBindings);
			std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
			imageInfos.resize(numberOfBindings);
			uint32_t bindingIndex = 0;
			for (const std::pair<DescriptorBindingConfig,uint32_t>& bindingConfig : attachmentReferenceBindings[frameIndex])
			{
				descriptorWrites[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[bindingIndex].dstSet = descriptorSetsPerFrame[frameIndex][bindingConfig.second];
				descriptorWrites[bindingIndex].dstBinding = bindingConfig.first.bindingSlot;				// Binding to update matches with binding on layout/shader)
				descriptorWrites[bindingIndex].dstArrayElement = 0;		// Index in array to update
				descriptorWrites[bindingIndex].descriptorType = static_cast<VkDescriptorType>(bindingConfig.first.type);
				descriptorWrites[bindingIndex].descriptorCount = bindingConfig.first.descriptorCount;		// Amount to update

				imageInfos.emplace_back();
				const bool isAttachmentReference = bindingConfig.first.isAttachmentReference;
				imageInfos[bindingIndex].resize(bindingConfig.first.descriptorCount);
				for (uint32_t descriptorIndex = 0; descriptorIndex < descriptorWrites[bindingIndex].descriptorCount; descriptorIndex++)
				{
					imageInfos[bindingIndex][descriptorIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfos[bindingIndex][descriptorIndex].imageView = framebufferImageViewsReference[frameIndex].at(bindingConfig.first.inputAttachmentHandle);
					imageInfos[bindingIndex][descriptorIndex].sampler = framebufferSampler;
				}
				descriptorWrites[bindingIndex].pImageInfo = imageInfos[bindingIndex].data();
				bindingIndex++;
			}
			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}
