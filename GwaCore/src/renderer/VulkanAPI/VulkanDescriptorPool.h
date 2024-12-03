#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice& logicalDevice, std::vector<VkBuffer>& buffers, const int FRAMES_IN_FLIGHT);
		VkDescriptorPool& getDescriptorPool()
		{
			return descriptorPool;
		}
		void cleanup(VkDevice logicalDevice);
	private:
		VkDescriptorPool descriptorPool;
	};
}
