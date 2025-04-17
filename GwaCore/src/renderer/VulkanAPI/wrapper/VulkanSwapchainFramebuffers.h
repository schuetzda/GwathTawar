#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapchain.h"
#include <span>

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
		std::span<VkFramebuffer> getFramebuffers() 
		{
			return std::span<VkFramebuffer>(swapchainFramebuffers_);
		}
	private:
		void createFramebuffers(const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swapchainExtent);

		std::vector<VkFramebuffer> swapchainFramebuffers_;
		VkDevice logicalDevice_{};
	};
}
