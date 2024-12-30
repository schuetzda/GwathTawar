#include "VulkanDescriptorSet.h"
#include <stdexcept>
#include <cassert>
namespace gwa
{
	VulkanDescriptorSet::VulkanDescriptorSet(VkDevice logicalDevice, VkDescriptorSetLayout descriptorSetLayout, const std::vector<VkBuffer>& uniformBuffers,
		const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize): logicalDevice_(logicalDevice)
	{
		//---DescriptorPool---

		// Type of descriptors + how many DESCRIPTORS, not descriptor sets (combined makes the pool size)
		VkDescriptorPoolSize vpPoolSize = {};
		vpPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		vpPoolSize.descriptorCount = static_cast<uint32_t> (uniformBuffers.size());
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes = { vpPoolSize };

		// Dynamic Model Pool for Dynamic Uniform Buffer. NOT IN USE, for reference only
		/*VkDescriptorPoolSize modelPoolSize = {};
		modelPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		modelPoolSize.descriptorCount = static_cast<uint32_t> (modelDynUniformBuffer.size());
		descriptorPoolSizes.push_back(modelPoolSize)
		*/

		// Data to create Descriptor Pool
		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.maxSets = static_cast<uint32_t> (MAX_FRAMES_IN_FLIGHT);		// Maximum number of Descriptor Sets that can be created from pool
		poolCreateInfo.poolSizeCount = static_cast<uint32_t> (descriptorPoolSizes.size());											// Amount of Pool Sizes being passed
		poolCreateInfo.pPoolSizes = descriptorPoolSizes.data();										// Pool Sizes to create pool with

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

			// Data about connection between binding and buffer
			VkWriteDescriptorSet vpSetWrite = {};
			vpSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vpSetWrite.dstSet = descriptorSets_[i];
			vpSetWrite.dstBinding = 0;				// Binding to update matches with binding on layout/shader)
			vpSetWrite.dstArrayElement = 0;		// Index in array to update
			vpSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			vpSetWrite.descriptorCount = 1;		// Amount to update
			vpSetWrite.pBufferInfo = &vpBufferInfo;// Information of buffer data to bind
			std::vector<VkWriteDescriptorSet> setWrites = { vpSetWrite };

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

			//List of Descriptor Set Writes


			// Update the descriptor sets with new buffer/ binding info
			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);

		}
	}

	void VulkanDescriptorSet::cleanup()
	{
		vkDestroyDescriptorPool(logicalDevice_, descriptorPool_, nullptr);
	}
}
