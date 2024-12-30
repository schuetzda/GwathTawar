#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanDevice.h"
namespace gwa
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool() = default;
		explicit VulkanCommandPool(const VulkanDevice* device);
		void cleanup();

		VkCommandPool getCommandPool() const
		{
			return commandPool_;
		}
	private:	
		VkCommandPool commandPool_;
		
		VkDevice logicalDevice_;
	};
}
