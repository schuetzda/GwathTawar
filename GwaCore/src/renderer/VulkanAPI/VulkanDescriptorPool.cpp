#include "VulkanDescriptorPool.h"
#include <stdexcept>
#include <cassert>

namespace gwa
{
	
	VulkanDescriptorPool::VulkanDescriptorPool(const VkDevice logicalDevice,const std::vector<VkBuffer>& buffers, const int FRAMES_IN_FLIGHT)
	{
		// Type of descriptors + how many DESCRIPTORS, not descriptor sets (combined makes the pool size)
		VkDescriptorPoolSize vpPoolSize = {};
		vpPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		vpPoolSize.descriptorCount = static_cast<uint32_t> (buffers.size());
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
		poolCreateInfo.maxSets = static_cast<uint32_t> (FRAMES_IN_FLIGHT);		// Maximum number of Descriptor Sets that can be created from pool
		poolCreateInfo.poolSizeCount = static_cast<uint32_t> (descriptorPoolSizes.size());											// Amount of Pool Sizes being passed
		poolCreateInfo.pPoolSizes = descriptorPoolSizes.data();										// Pool Sizes to create pool with

		// Create Descriptor Pool
		VkResult result = vkCreateDescriptorPool(logicalDevice, &poolCreateInfo, nullptr, &descriptorPool);
		assert(result == VK_SUCCESS);
	}
	void VulkanDescriptorPool::cleanup(VkDevice logicalDevice)
	{
		vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	}
}
