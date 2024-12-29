#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>

	namespace gwa
	{
		class VulkanPipeline
		{
		public:
			VulkanPipeline()= default;
			void init(VkDevice logicalDevice, uint32_t stride, const std::vector<uint32_t>& attributeDescriptionOffsets, VkRenderPass renderPass, const VkExtent2D& swapchainExtent, const VkPushConstantRange& pushConstantRange, VkDescriptorSetLayout descriptorSetLayout);
			void cleanup();

		private:
			VkShaderModule createShaderModule(const std::vector<char>& code);

			VkPipeline graphicsPipeline_;
			VkPipelineLayout pipelineLayout_;

			VkDevice vkLogicalDevice_;
	};
	}


