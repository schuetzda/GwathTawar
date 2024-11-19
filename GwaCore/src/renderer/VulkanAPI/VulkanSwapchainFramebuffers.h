#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <renderer/VulkanAPI/VulkanSwapchain.h>

namespace gwa
{
	class VulkanSwapchainFramebuffers
	{
	public:
		VulkanSwapchainFramebuffers(VkDevice& logicalDevice, std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass& renderPass,
			VkImageView& depthBufferImageView, VkExtent2D swpachainExtent);
		void cleanup(VkDevice& logicalDevice);
	private:
		std::vector<VkFramebuffer> swapchainFramebuffers;

		VulkanSwapchainFramebuffers(VkDevice& logicalDevice, std::vector<VkImage>& swapchainImages, VkRenderPass& renderPass, VkImageView& depthBufferImageView, VkExtent2D swpachainExtent);

	};
}
