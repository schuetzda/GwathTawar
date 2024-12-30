#include "VulkanDescriptorSetLayout.h"
#include <vector>
#include <stdexcept>
#include <cassert>
namespace gwa
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice logicalDevice):logicalDevice_(logicalDevice)
	{
		// MVPMat Binding Info
		VkDescriptorSetLayoutBinding vpLayoutBinding = {};
		vpLayoutBinding.binding = 0;						// Where is the data being bound to?
		vpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		vpLayoutBinding.descriptorCount = 1;
		vpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		vpLayoutBinding.pImmutableSamplers = nullptr;		// For Texture: Can make sampler unchangeable by specifying layout

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { vpLayoutBinding };

		/* NOT IN USE, for reference of Dynamic UBO
		// Model Binding Info
		VkDescriptorSetLayoutBinding modelLayoutBinding = {};
		modelLayoutBinding.binding = 1;
		modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		modelLayoutBinding.descriptorCount = 1;
		modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		modelLayoutBinding.pImmutableSamplers = nullptr;
		layoutBindings.push_back(modelLayoutBinding);
		*/

		// Create Descriptor Set Layout with given bindings
		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutCreateInfo.pBindings = layoutBindings.data();				// array of binding infos

		VkResult result = vkCreateDescriptorSetLayout(logicalDevice_, &layoutCreateInfo, nullptr, &descriptorSetLayout_);
		assert(result == VK_SUCCESS);

	}
	void VulkanDescriptorSetLayout::cleanup()
	{
		vkDestroyDescriptorSetLayout(logicalDevice_, descriptorSetLayout_,nullptr);
	}
}
