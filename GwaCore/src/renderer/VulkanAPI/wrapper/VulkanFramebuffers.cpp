#include "VulkanFramebuffers.h"
#include <stdexcept>
#include <array>
#include <cassert>


namespace gwa::renderer
{
	VulkanFramebuffers::VulkanFramebuffers(VkDevice logicalDevice, const std::span<VulkanImageViewCollection> framebufferImageViews, VkRenderPass renderPass, VkExtent2D framebufferExtent)
	{
		createFramebuffers(logicalDevice, framebufferImageViews, renderPass, framebufferExtent);
	}
	void VulkanFramebuffers::recreateFramebuffer(VkDevice logicalDevice, const std::span<VulkanImageViewCollection> framebufferImageViews, VkRenderPass renderPass, VkExtent2D swapchainExtent)
	{
		cleanup(logicalDevice);
		createFramebuffers(logicalDevice, framebufferImageViews, renderPass, swapchainExtent);
	}
	
	void VulkanFramebuffers::createFramebuffers(VkDevice logicalDevice,const std::span<VulkanImageViewCollection> framebufferImageViews, VkRenderPass renderPass, VkExtent2D framebufferExtent)
	{
		framebuffers.resize(framebufferImageViews.size());
		for (size_t i = 0; i < framebuffers.size(); i++)
		{
			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(framebufferImageViews[i].getImageViews().size());
			framebufferCreateInfo.pAttachments = framebufferImageViews[i].getImageViews().data();
			framebufferCreateInfo.width = framebufferExtent.width;
			framebufferCreateInfo.height = framebufferExtent.height;
			framebufferCreateInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &framebuffers[i]);
			assert(result == VK_SUCCESS);
		}
	}
	void VulkanFramebuffers::cleanup(VkDevice logicalDevice) 
	{
		for (auto framebuffer : framebuffers)
		{
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}
	}
}
