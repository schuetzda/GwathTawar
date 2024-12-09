#include "VulkanSwapchainFramebuffers.h"
#include <stdexcept>
#include <array>
#include <cassert>
namespace gwa
{
	VulkanSwapchainFramebuffers::VulkanSwapchainFramebuffers(VkDevice logicalDevice,const std::vector<VulkanSwapchainImage>& swapchainImages, VkRenderPass renderPass,
		VkImageView depthBufferImageView, VkExtent2D swapchainExtent)
	{
		swapchainFramebuffers.resize(swapchainImages.size());

		for (size_t i = 0; i < swapchainFramebuffers.size(); ++i)
		{
			const uint32_t attachmentsSize = 2;
			std::array<VkImageView,attachmentsSize> attachments{
				swapchainImages[i].imageView,
				depthBufferImageView
			};

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;
			framebufferCreateInfo.attachmentCount = attachmentsSize;
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = swapchainExtent.width;
			framebufferCreateInfo.height = swapchainExtent.height;
			framebufferCreateInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]);
			assert(result == VK_SUCCESS);
		}
	}
	void VulkanSwapchainFramebuffers::cleanup(VkDevice logicalDevice) const
	{
		for (auto framebuffer : swapchainFramebuffers)
		{
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}
	}
}
