#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>

	namespace gwa
	{
		class VulkanPipeline
		{
		public:
			VulkanPipeline(VkDevice& logicalDevice, uint32_t stride, std::vector<uint32_t>& attributeDescriptionOffsets, VkRenderPass& renderPass, VkExtent2D& swapchainExtent, VkPushConstantRange& pushConstantRange, VkDescriptorSetLayout& descriptorSetLayout);
			void cleanup(VkDevice& logicalDevice);
		private:
			VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice& logicalDevice);
			VkPipelineLayout pipelineLayout;
			VkPipeline graphicsPipeline;
	};
	}


