#include "VulkanUniformBuffers.h"
#include "VulkanUtility.h"
#include <stdexcept>
#include <cassert>
namespace gwa::renderer
{
	VulkanUniformBuffers::VulkanUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint64_t uniformBufferSize, uint32_t maxFramesInFlight, ResourceAttachment uboResource) :uboResource(uboResource)
	{
		VkDeviceSize vpBufferSize = uniformBufferSize;

		// One uniform buffer for each image (and by extension, command buffer)
		uniformBuffers_.resize(maxFramesInFlight);
		uniformBufferMemory_.resize(maxFramesInFlight);

		// Create Uniform buffers
		for (size_t i = 0; i < maxFramesInFlight; ++i)
		{
			createBuffer(logicalDevice, physicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformBuffers_[i], &uniformBufferMemory_[i]);	
		}
	}

	void VulkanUniformBuffers::createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
		VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) const
	{
		// Create Vertex Buffer
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = bufferUsage;							// Multiple types of buffers possible
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// Similar to swapchain images, can share vertex buffers
		
		VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, buffer);

		assert(result == VK_SUCCESS);
		// Get Buffer memory requirements
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memRequirements);

		// Allocate memory to buffer
		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = vulkanutil::findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flag
			bufferProperties);							//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : CPU can interact with memory
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Allows placement of data straight into buffer after mapping
		result = vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, bufferMemory);

		assert(result == VK_SUCCESS);
		// Allocate memory to given Vertex Buffer
		vkBindBufferMemory(logicalDevice, *buffer, *bufferMemory, 0);

	}
	void VulkanUniformBuffers::updateUniformBuffers(VkDevice logicalDevice, uint32_t imageIndex, uint64_t uniformBufferSize, const void* uniformBufferData)
	{
		void* data;
		vkMapMemory(logicalDevice, uniformBufferMemory_[imageIndex], 0, uniformBufferSize, 0, &data);
		memcpy(data, uniformBufferData, uniformBufferSize);
		vkUnmapMemory(logicalDevice, uniformBufferMemory_[imageIndex]);

	}
	void VulkanUniformBuffers::cleanup(VkDevice logicalDevice)
	{
		for (size_t i = 0; i < uniformBuffers_.size(); ++i)
		{
			vkDestroyBuffer(logicalDevice, uniformBuffers_[i], nullptr);
			vkFreeMemory(logicalDevice, uniformBufferMemory_[i], nullptr);
			//vkDestroyBuffer(mainDevice.logicalDevice, modelDynUniformBuffer[i], nullptr);
			//vkFreeMemory(mainDevice.logicalDevice, modelDynUniformBufferMemory[i], nullptr);
		}
	}
}
