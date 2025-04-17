#pragma once
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>

	namespace gwa
	{
		class VulkanPipeline
		{
		public:
			VulkanPipeline() = default;
			VulkanPipeline(VkDevice logicalDevice, uint32_t stride, const std::array<uint32_t,2>& attributeDescriptionOffsets, VkRenderPass renderPass, 
				const VkExtent2D& swapchainExtent, const VkPushConstantRange& pushConstantRange, VkDescriptorSetLayout descriptorSetLayout);
			void cleanup();
			VkPipeline getPipeline() const
			{
				return pipeline_;
			}
			VkPipelineLayout getPipelineLayout() const
			{
				return pipelineLayout_;
			}
		private:
			VkShaderModule createShaderModule(const std::vector<char>& code);

			VkPipeline pipeline_{};
			VkPipelineLayout pipelineLayout_{};

			VkDevice logicalDevice_{};
	};
	}


