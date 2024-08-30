#include <vulkan/vulkan.h>
#pragma once
namespace gwa
{
	class VulkanDescriptorSetLayout
	{
	public:
		explicit VulkanDescriptorSetLayout(VkDevice& vkLogicalDevice);
		VkDescriptorSetLayout& getDescriptorSetLayout()
		{
			return vkDescriptorSetLayout;
		}
		void cleanup(VkDevice vkLogicalDevice);
	private:
		VkDescriptorSetLayout vkDescriptorSetLayout;
	};
}
