#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include "VulkanDevice.h"
namespace gwa
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass() = default;
		VulkanRenderPass(const VulkanDevice* const device, VkFormat swapchainImageFormat);
		~VulkanRenderPass() = default;

		void cleanup();

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

		VkDevice logicalDevice_{};
	};
}
