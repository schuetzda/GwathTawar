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
		void recreateSwapchain(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swapchainExtent);
		void cleanup(VkDevice logicalDevice);

		std::vector<VkFramebuffer> swapchainFramebuffers;
	private:
		void createFramebuffers(VkDevice logicalDevice, const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
			VkImageView depthBufferImageView, VkExtent2D swapchainExtent);

	};
}
