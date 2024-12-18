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
		void cleanup(VkDevice vkLogicalDevice);

		VkRenderPass vkRenderPass;
		VkFormat depthFormat;
	private:
		VkFormat chooseSupportedFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) const;
		
	};
}
