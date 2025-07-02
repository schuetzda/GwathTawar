#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include "VulkanDevice.h"
#include <renderer/rendergraph/RenderGraph.h>
namespace gwa::renderer
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass() = default;
		VulkanRenderPass(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkFormat swapchainImageFormat, const RenderPassConfig& renderPassConfig, const std::map<size_t, RenderAttachment>& attachments, VkFormat depthFormat);
		~VulkanRenderPass() = default;

		void cleanup(VkDevice logicalDevice);

		VkRenderPass getRenderPass() const
		{
			return vkRenderPass_;
		}

		uint32_t getOutputAttachmentCounts() const
		{
			return outputAttachmentsCount;
		}

	private:
		VkRenderPass vkRenderPass_{};
		uint32_t outputAttachmentsCount{ 0 };
	};

}
