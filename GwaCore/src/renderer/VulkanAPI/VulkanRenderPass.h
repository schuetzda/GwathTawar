#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
namespace gwa
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VkPhysicalDevice vkPhysicalDevice, VkDevice logicalDevice, VkFormat swapchainImageFormat);
		~VulkanRenderPass();
		VkRenderPass& getRenderPass()
		{
			return vkRenderPass;
		}
		VkFormat& getDepthFormat()
		{
			return depthFormat;
		}
		void cleanup(VkDevice vkLogicalDevice);
	private:
		VkFormat chooseSupportedFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) const;
		
	VkRenderPass vkRenderPass;
		VkFormat depthFormat;
	};
}
