#pragma once
#include <vulkan/vulkan.h>
namespace gwa
{
	class VulkanImageSampler
	{
	public:
		VulkanImageSampler() = default;
		VulkanImageSampler(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		
		void cleanup();
		VkSampler getImageSampler() const
		{
			return textureSampler_;
		}
	private:
		VkSampler textureSampler_;
		VkDevice logicalDevice_;
	};
}
