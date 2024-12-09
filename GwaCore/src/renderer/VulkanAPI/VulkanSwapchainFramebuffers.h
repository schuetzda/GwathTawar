#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <renderer/VulkanAPI/VulkanSwapchain.h>

namespace gwa
{
	class VulkanSwapchainFramebuffers
	{
	public:
		VulkanSwapchainFramebuffers(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swpachainExtent);
		void cleanup(VkDevice logicalDevice) const;
	private:
		std::vector<VkFramebuffer> swapchainFramebuffers;
	};
}
