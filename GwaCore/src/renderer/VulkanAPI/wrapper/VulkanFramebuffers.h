#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <span>
#include "RendererConfig.h"
#include "VulkanSwapchain.h"
#include "VulkanImageViewCollection.h"

namespace gwa::renderer
{
	class VulkanFramebuffers
	{
	public:
		VulkanFramebuffers() = default;
		VulkanFramebuffers(VkDevice logicalDevice, const std::span<VulkanImageViewCollection> framebufferImageViews, VkRenderPass renderPass, VkExtent2D framebufferExtent);

		void recreateFramebuffer(VkDevice logicalDevice, const std::span<VulkanImageViewCollection> framebufferImageViews, VkRenderPass renderPass, const std::span<VulkanImageViewCollection> depthBufferImageViews, VkExtent2D swapchainExtent);

		void cleanup(VkDevice logicalDevice);
		std::vector<VkFramebuffer>& getFramebuffers()
		{
			return framebuffers;
		}
	private:
		void createFramebuffers(VkDevice logicalDevice, const std::span<VulkanImageViewCollection> framebufferImageViews, VkRenderPass renderPass, VkExtent2D framebufferExtent);

		std::vector<VkFramebuffer> framebuffers{};
	};
}
