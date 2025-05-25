#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>
#include <span>
#include "renderer/rendergraph/PipelineBuilder.h"

	namespace gwa::renderer
	{
		class VulkanPipeline
		{
		public:
			VulkanPipeline() = default;
			VulkanPipeline(VkDevice logicalDevice, const renderer::PipelineConfig& config, VkRenderPass renderPass, const VkPushConstantRange& pushConstantRange, std::span<const VkDescriptorSetLayout> descriptorSetLayout);
			VulkanPipeline(VkDevice logicalDevice, uint32_t stride, const std::array<uint32_t,2>& attributeDescriptionOffsets, VkRenderPass renderPass,
				const VkExtent2D& swapchainExtent, const VkPushConstantRange& pushConstantRange, VkDescriptorSetLayout descriptorSetLayout);

			void cleanup(VkDevice logicalDevice);

			VkPipeline getPipeline() const
			{
				return pipeline_;
			}
			VkPipelineLayout getPipelineLayout() const
			{
				return pipelineLayout_;
			}
		private:
			VkShaderModule createShaderModule(VkDevice logicalDevice, const std::vector<char>& code);

			VkPipeline pipeline_{};
			VkPipelineLayout pipelineLayout_{};
	};
	}


