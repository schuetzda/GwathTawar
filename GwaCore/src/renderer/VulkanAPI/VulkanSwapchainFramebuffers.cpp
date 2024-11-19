#include "VulkanSwapchainFramebuffers.h"
#include <stdexcept>
namespace gwa
{
	VulkanSwapchainFramebuffers::VulkanSwapchainFramebuffers(VkDevice& logicalDevice, std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass& renderPass,
		VkImageView& depthBufferImageView, VkExtent2D swapchainExtent)
	{
		swapchainFramebuffers.resize(swapchainImages.size());

		for (size_t i = 0; i < swapchainFramebuffers.size(); ++i)
		{
			const uint32_t attachmentsSize = 2;
			VkImageView attachments[attachmentsSize] = {
				swapchainImages[i].imageView,
				depthBufferImageView
			};

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;
			framebufferCreateInfo.attachmentCount = attachmentsSize;
			framebufferCreateInfo.pAttachments = attachments;
			framebufferCreateInfo.width = swapchainExtent.width;
			framebufferCreateInfo.height = swapchainExtent.height;
			framebufferCreateInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]);
			if (result != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create a framebuffer!");
			}
		}
	}
	void VulkanSwapchainFramebuffers::cleanup(VkDevice& logicalDevice)
	{
		for (auto framebuffer : swapchainFramebuffers)
		{
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}
	}
}
