#include <vulkan/vulkan.h>
#pragma once
namespace gwa
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout() = default;
		explicit VulkanDescriptorSetLayout(VkDevice logicalDevice);
		void cleanup();
		
		VkDescriptorSetLayout getDescriptorSetLayout()
		{
			return descriptorSetLayout_;
		}
	private:
		VkDescriptorSetLayout descriptorSetLayout_{};
		VkDevice logicalDevice_{};
	};
}
