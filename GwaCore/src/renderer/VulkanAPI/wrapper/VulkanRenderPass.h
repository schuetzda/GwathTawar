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
		VulkanRenderPass(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkFormat swapchainImageFormat, const RenderPassConfig& renderPassConfig, const std::map<size_t, Attachment>& attachments);
		VulkanRenderPass(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkFormat swapchainImageFormat);
		~VulkanRenderPass() = default;

		void cleanup(VkDevice logicalDevice);

		VkRenderPass getRenderPass() const
		{
			return vkRenderPass_;
		}

		VkFormat getDepthFormat() const
		{
			return depthFormat_;
		}

	private:
		VkFormat chooseSupportedFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) const;



		VkRenderPass vkRenderPass_{};
		VkFormat depthFormat_{};
	};
}
