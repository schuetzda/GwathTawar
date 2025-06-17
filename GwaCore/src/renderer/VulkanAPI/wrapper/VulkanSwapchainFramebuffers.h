#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapchain.h"
#include <span>
#include "RendererConfig.h"

namespace gwa::renderer
{
	class VulkanSwapchainFramebuffers
	{
	public:
		VulkanSwapchainFramebuffers() = default;
		VulkanSwapchainFramebuffers(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swpachainExtent);

		void recreateSwapchain(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass, VkImageView depthBufferImageView, VkExtent2D swapchainExtent);
		void cleanup(VkDevice logicalDevice);
		std::span<VkFramebuffer> getFramebuffers() 
		{
			return std::span<VkFramebuffer>(swapchainFramebuffers_);
		}
	private:
		void createFramebuffers(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass, VkImageView depthBufferImageView, VkExtent2D swapchainExtent);

		std::vector<VkFramebuffer> swapchainFramebuffers_{};
	};
}
