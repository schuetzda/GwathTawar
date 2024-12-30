#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapchain.h"

namespace gwa
{
	class VulkanSwapchainFramebuffers
	{
	public:
		VulkanSwapchainFramebuffers() = default;
		VulkanSwapchainFramebuffers(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swpachainExtent);

		void recreateSwapchain(const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swapchainExtent);
		void cleanup();
		const std::vector<VkFramebuffer>& getFramebuffers() const
		{
			return swapchainFramebuffers_;
		}
	private:
		void createFramebuffers(const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swapchainExtent);

		std::vector<VkFramebuffer> swapchainFramebuffers_;
		VkDevice logicalDevice_{};
	};
}
