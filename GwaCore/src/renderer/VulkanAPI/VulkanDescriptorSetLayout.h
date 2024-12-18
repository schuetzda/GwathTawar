#include <vulkan/vulkan.h>
#pragma once
namespace gwa
{
	class VulkanDescriptorSetLayout
	{
	public:
		explicit VulkanDescriptorSetLayout(VkDevice vkLogicalDevice);
		void cleanup(VkDevice vkLogicalDevice);

		VkDescriptorSetLayout vkDescriptorSetLayout;
	};
}
