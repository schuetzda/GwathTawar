#include "VulkanUniformBuffers.h"
#include "VulkanUtility.h"
#include <stdexcept>
#include <cassert>
namespace gwa
{
	VulkanUniformBuffers::VulkanUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint64_t uniformBufferSize, const int IMAGE_COUNT) :imageCount_(IMAGE_COUNT), logicalDevice_(logicalDevice)
	{
		VkDeviceSize vpBufferSize = uniformBufferSize;
		// NOT IN USE, for Dynamic UBO
		//VkDeviceSize modelBufferSize = modelUniformAlignment * MAX_OBJECTS;

		// One uniform buffer for each image (and by extension, command buffer)
		uniformBuffers_.resize(IMAGE_COUNT);
		uniformBufferMemory_.resize(IMAGE_COUNT);
		//modelDynUniformBuffer.resize(swapchainImages.size());
		//modelDynUniformBufferMemory.resize(swapchainImages.size());

		// Create Uniform buffers
		for (size_t i = 0; i < IMAGE_COUNT; ++i)
		{
			createBuffer(physicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformBuffers_[i], &uniformBufferMemory_[i]);
			//createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &modelDynUniformBuffer[i], &modelDynUniformBufferMemory[i]);
		}
	}

	void VulkanUniformBuffers::createBuffer(VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
		VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) const
	{
		// Create Vertex Buffer
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = bufferUsage;							// Multiple types of buffers possible
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// Similar to swapchain images, can share vertex buffers
		
		VkResult result = vkCreateBuffer(logicalDevice_, &bufferInfo, nullptr, buffer);

		assert(result == VK_SUCCESS);
		// Get Buffer memory requirements
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice_, *buffer, &memRequirements);

		// Allocate memory to buffer
		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = vulkanutil::findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flag
			bufferProperties);							//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : CPU can interact with memory
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Allows placement of data straight into buffer after mapping
		result = vkAllocateMemory(logicalDevice_, &memoryAllocInfo, nullptr, bufferMemory);

		assert(result == VK_SUCCESS);
		// Allocate memory to given Vertex Buffer
		vkBindBufferMemory(logicalDevice_, *buffer, *bufferMemory, 0);

	}
	void VulkanUniformBuffers::updateUniformBuffers(uint32_t imageIndex, uint64_t uniformBufferSize, const void* uniformBufferData)
	{
		void* data;
		const glm::mat4 uoMat = *static_cast<const glm::mat4*>(uniformBufferData);
		vkMapMemory(logicalDevice_, uniformBufferMemory_[imageIndex], 0, uniformBufferSize, 0, &data);
		memcpy(data, uniformBufferData, uniformBufferSize);
		const glm::mat4 uboMat = *static_cast<const glm::mat4*>(data);
		vkUnmapMemory(logicalDevice_, uniformBufferMemory_[imageIndex]);

	}
	void VulkanUniformBuffers::cleanup()
	{
		for (size_t i = 0; i < imageCount_; ++i)
		{
			vkDestroyBuffer(logicalDevice_, uniformBuffers_[i], nullptr);
			vkFreeMemory(logicalDevice_, uniformBufferMemory_[i], nullptr);
			//vkDestroyBuffer(mainDevice.logicalDevice, modelDynUniformBuffer[i], nullptr);
			//vkFreeMemory(mainDevice.logicalDevice, modelDynUniformBufferMemory[i], nullptr);
		}
	}
}
