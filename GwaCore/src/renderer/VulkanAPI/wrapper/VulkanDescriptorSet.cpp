#include "VulkanDescriptorSet.h"
#include <stdexcept>
#include <cassert>
#include <array>
namespace gwa
{
	VulkanDescriptorSet::VulkanDescriptorSet(VkDevice logicalDevice, VkDescriptorSetLayout descriptorSetLayout, const std::vector<VkBuffer>& uniformBuffers,
		const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize, VkImageView textureImageView, VkSampler textureSampler): logicalDevice_(logicalDevice)
	{
		//---DescriptorPool---

		// Type of descriptors + how many DESCRIPTORS, not descriptor sets (combined makes the pool size)
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		// Dynamic Model Pool for Dynamic Uniform Buffer. NOT IN USE, for reference only
		/*VkDescriptorPoolSize modelPoolSize = {};
		modelPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		modelPoolSize.descriptorCount = static_cast<uint32_t> (modelDynUniformBuffer.size());
		descriptorPoolSizes.push_back(modelPoolSize)
		*/

		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolCreateInfo.pPoolSizes = poolSizes.data();
		poolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		// Create Descriptor Pool
		VkResult result = vkCreateDescriptorPool(logicalDevice, &poolCreateInfo, nullptr, &descriptorPool_);
		assert(result == VK_SUCCESS);

		//---Descriptor Set---
		descriptorSets_.resize(MAX_FRAMES_IN_FLIGHT);

		//Each sets has the same layout
		std::vector<VkDescriptorSetLayout> setLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

		VkDescriptorSetAllocateInfo setAllocInfo = {};
		setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocInfo.descriptorPool = descriptorPool_;
		setAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		setAllocInfo.pSetLayouts = setLayouts.data();

		//Allocate descriptorSet 
		result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, descriptorSets_.data());
		assert(result == VK_SUCCESS);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			// buffer info and data offset info
			VkDescriptorBufferInfo vpBufferInfo = {};
			vpBufferInfo.buffer = uniformBuffers[i];		// Buffer to get data from
			vpBufferInfo.offset = 0;						// position of start of data
			vpBufferInfo.range = dataSize;			// size of daa

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			// Data about connection between binding and buffer
			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets_[i];
			descriptorWrites[0].dstBinding = 0;				// Binding to update matches with binding on layout/shader)
			descriptorWrites[0].dstArrayElement = 0;		// Index in array to update
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;		// Amount to update
			descriptorWrites[0].pBufferInfo = &vpBufferInfo;// Information of buffer data to bind

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets_[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			/* NOT IN USE, for reference of Dynamic UBO
			VkDescriptorBufferInfo modelBufferInfo = {};
			modelBufferInfo.buffer = modelDynUniformBuffer[i];
			modelBufferInfo.offset = 0;
			modelBufferInfo.range = modelUniformAlignment;

			VkWriteDescriptorSet modelSetWrite = {};
		VulkanDescriptorSet(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT, uint64_t size);
			modelSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			modelSetWrite.dstSet = descriptorSets[i];
			modelSetWrite.dstBinding = 1;
			modelSetWrite.dstArrayElement = 0;
			modelSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			modelSetWrite.descriptorCount = 1;
			modelSetWrite.pBufferInfo = &modelBufferInfo;
			setWrites.push_back(modelSetWrite);*/

			// Update the descriptor sets with new buffer/ binding info
			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		}
	}

	void VulkanDescriptorSet::cleanup()
	{
		vkDestroyDescriptorPool(logicalDevice_, descriptorPool_, nullptr);
	}
}
