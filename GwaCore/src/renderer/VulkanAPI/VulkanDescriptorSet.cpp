#include "VulkanDescriptorSet.h"
#include <stdexcept>
namespace gwa
{
	VulkanDescriptorSet::VulkanDescriptorSet(VkDevice& logicalDevice, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool,std::vector<VkBuffer>& uniformBuffers, const int MAX_FRAMES_IN_FLIGHT, uint64_t dataSize)
	{
		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		//Each sets has the same layout
		std::vector<VkDescriptorSetLayout> setLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

		VkDescriptorSetAllocateInfo setAllocInfo = {};
		setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocInfo.descriptorPool = descriptorPool;
		setAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		setAllocInfo.pSetLayouts = setLayouts.data();

		//Allocate descriptorSet 
		VkResult result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, descriptorSets.data());
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate Descriptor Sets!");
		}

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
			vpSetWrite.dstSet = descriptorSets[i];
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
}
