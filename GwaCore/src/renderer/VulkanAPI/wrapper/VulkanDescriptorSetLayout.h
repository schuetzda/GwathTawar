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
		VulkanDescriptorSetLayout(VkDevice logicalDevice, const std::vector<DescriptorSetConfig>& descriptorSetsConfig);
		void cleanup(VkDevice logicalDevice);
		
		const std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts()
		{
			return descriptorSetLayouts;
		}
	private:

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	};
}
