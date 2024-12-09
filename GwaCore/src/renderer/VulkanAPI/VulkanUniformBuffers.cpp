#include "VulkanUniformBuffers.h"
#include "MemoryType.h"
#include <stdexcept>
#include <cassert>
namespace gwa
{
	VulkanUniformBuffers::VulkanUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint64_t uniformBufferSize, const int MAX_FRAMES_IN_FLIGHT) : MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT)
	{
		VkDeviceSize vpBufferSize = uniformBufferSize;
		// NOT IN USE, for Dynamic UBO
		//VkDeviceSize modelBufferSize = modelUniformAlignment * MAX_OBJECTS;

		// One uniform buffer for each image (and by extension, command buffer)
		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBufferMemory.resize(MAX_FRAMES_IN_FLIGHT);
		//modelDynUniformBuffer.resize(swapchainImages.size());
		//modelDynUniformBufferMemory.resize(swapchainImages.size());

		// Create Uniform buffers
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			createBuffer(physicalDevice, logicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformBuffers[i], &uniformBufferMemory[i]);
			//createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &modelDynUniformBuffer[i], &modelDynUniformBufferMemory[i]);
		}
	}

	void VulkanUniformBuffers::createBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
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
		memoryAllocInfo.memoryTypeIndex = MemoryType::findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flag
			bufferProperties);							//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : CPU can interact with memory
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Allows placement of data straight into buffer after mapping
		result = vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, bufferMemory);

		assert(result == VK_SUCCESS);
		// Allocate memory to given Vertex Buffer
		vkBindBufferMemory(logicalDevice, *buffer, *bufferMemory, 0);

	}
	void VulkanUniformBuffers::cleanup(VkDevice logicalDevice)
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
			vkFreeMemory(logicalDevice, uniformBufferMemory[i], nullptr);
			//vkDestroyBuffer(mainDevice.logicalDevice, modelDynUniformBuffer[i], nullptr);
			//vkFreeMemory(mainDevice.logicalDevice, modelDynUniformBufferMemory[i], nullptr);
		}
	}
}
