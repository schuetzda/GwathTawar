#include <vulkan/vulkan.h>
#pragma once
namespace gwa
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout() = default;
		void init(VkDevice vkLogicalDevice);
		void cleanup();
		
		VkDescriptorSetLayout getDescriptorSetLayout()
		{
			return vkDescriptorSetLayout_;
		}
	private:
		VkDescriptorSetLayout vkDescriptorSetLayout_;
		VkDevice vkLogicalDevice_;
	};
}
