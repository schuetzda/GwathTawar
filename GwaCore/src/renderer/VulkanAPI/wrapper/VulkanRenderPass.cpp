/* Renderpass
All rendering happens inside a Renderpass. A renderpass will render into a Framebuffer.
It can contain multiple subpasses. The image layout of an image changes inside a Renderpass.
*/
#include "VulkanRenderPass.h"
#include <array>
#include <stdexcept>
#include <cassert>

namespace gwa
{
	VkFormat VulkanRenderPass::chooseSupportedFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) const
	{
		for (VkFormat format : formats)
		{
			// Get properties for given format o nthis device
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &properties);
			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags) {
				return format;
			}
		}
		assert(false);
		return formats[0];
	}

	VulkanRenderPass::VulkanRenderPass(const VulkanDevice* const device, VkFormat swapchainImageFormat):logicalDevice_(device->getLogicalDevice())
	{
		// Color Attachments of render pass. All subpasses have access to this.
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapchainImageFormat;						// Format of attachment
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;					// for multisampling
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;				// Describes what to do with attachment before rendering
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;				// Describes what to do with attachment after rendering
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;	// Not using it right now so we don't care
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// Framebuffer data will be stored as an image, but images can be given different data layouts
		// to give optimal use foor certain operations
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;			// Image data layout before render pass starts
		// There is an subpass format inbetween (see colorAttachment layout)
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;		// Image data layout after render pass (to change to)
		// Depth attachment of render pass
		VkAttachmentDescription depthAttachment = {};
		depthFormat_ = chooseSupportedFormat(
			device->getPhysicalDevice(),
			{ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		depthAttachment.format = depthFormat_;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //TODO sth in the tutorial at 40 min not done
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//REFERENCES
		// Attachment reference uses an attachment index that refers to index in the attachment list passed to renderPassCreateInfo
		VkAttachmentReference colorAttachementReference = {};
		colorAttachementReference.attachment = 0;
		colorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Depth Attachment reference
		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Information about a particular subpass the Render Pas is using
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		// Pipeline type subpass is to be bound to (change for raytracing)
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachementReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		// Need to determine when layout transitions occur using subpass dependencies
		const uint32_t subpassDepSize = 2;
		std::array<VkSubpassDependency, subpassDepSize> subpassDependencies;
		//Conversion from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkAccessFlagBits.html

		// Transition must happen after ...
		subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;			// Meaning outside of renderpass
		subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// pipeline stage
		subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;	// memory access

		// Transition must happen before ...
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[0].dependencyFlags = 0;


		//Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		// Transition must happen after ...
		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// pipeline stage
		subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// memory access

		// Transition must happen before ...
		subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[1].dependencyFlags = 0;

		std::array<VkAttachmentDescription, 2> renderPassAttachments = { colorAttachment, depthAttachment };

		// Create Info for renderpass
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(renderPassAttachments.size());
		renderPassCreateInfo.pAttachments = renderPassAttachments.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = subpassDepSize;
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		VkResult result = vkCreateRenderPass(logicalDevice_, &renderPassCreateInfo, nullptr, &vkRenderPass_);
		assert(result == VK_SUCCESS);
	}

	void VulkanRenderPass::cleanup()
	{
		vkDestroyRenderPass(logicalDevice_, vkRenderPass_, nullptr);
	}

}
