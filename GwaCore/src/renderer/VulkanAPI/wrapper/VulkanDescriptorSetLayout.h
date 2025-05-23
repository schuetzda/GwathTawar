#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/rendergraph/DescriptorSetConfigurator.h"
#pragma once
namespace gwa::renderer
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout() = default;
		explicit VulkanDescriptorSetLayout(VkDevice logicalDevice);
		VulkanDescriptorSetLayout(VkDevice logicalDevice, const std::vector<DescriptorSetConfig>& descriptorSetsConfig);
		void cleanup(VkDevice logicalDevice);
		
		const std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts()
		{
			return descriptorSetLayouts;
		}
	private:
		VkDescriptorSetLayout createBindlessDescriptorSetLayout(VkDevice logicalDevice, const DescriptorSetConfig& descriptorConfig);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		VkDevice logicalDevice_{};
	};
}
