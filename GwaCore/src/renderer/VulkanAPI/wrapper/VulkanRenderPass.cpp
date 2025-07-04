/* Renderpass
All rendering happens inside a Renderpass. A renderpass will render into a Framebuffer.
It can contain multiple subpasses. The image layout of an image changes inside a Renderpass.
*/
#include "VulkanRenderPass.h"
#include "renderer/rendergraph/RenderGraphData.h"
#include <array>
#include <stdexcept>
#include <cassert>

namespace gwa::renderer
{
	
	VulkanRenderPass::VulkanRenderPass(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkFormat swapchainImageFormat, const RenderPassConfig& renderPassConfig, const std::map<size_t,RenderAttachment>& attachments, VkFormat depthFormat= VkFormat::VK_FORMAT_UNDEFINED)
	{
		outputAttachmentsCount = static_cast<uint32_t>(renderPassConfig.outputAttachmentHandles.size());
		std::vector<VkAttachmentDescription> attDescriptions;
		attDescriptions.resize(outputAttachmentsCount);
		std::vector<VkAttachmentReference> attReferences;
		attReferences.resize(outputAttachmentsCount);

		for (uint32_t i = 0; i < outputAttachmentsCount; i++)
		{
			const size_t attachmentIndex = renderPassConfig.outputAttachmentHandles[i];
			const renderer::RenderAttachment& curAttachment = attachments.at(attachmentIndex);

			attDescriptions[i].format = curAttachment.format == renderer::Format::FORMAT_SWAPCHAIN_IMAGE_FORMAT ? swapchainImageFormat : static_cast<VkFormat>(curAttachment.format);
			attDescriptions[i].samples = static_cast<VkSampleCountFlagBits>(curAttachment.sample);
			attDescriptions[i].loadOp = static_cast<VkAttachmentLoadOp>(curAttachment.loadOp);
			attDescriptions[i].storeOp = static_cast<VkAttachmentStoreOp>(curAttachment.storeOp);
			attDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attDescriptions[i].initialLayout = static_cast<VkImageLayout>(curAttachment.initialLayout);
			attDescriptions[i].finalLayout = static_cast<VkImageLayout>(curAttachment.finalLayout);

			attReferences[i].attachment = i;
			attReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		// Valid depth stencil attachment
		const bool depthBufferingEnabled = renderPassConfig.depthStencilAttachmentHandle < attachments.size();
		VkAttachmentReference depthAttachmentReference = {};

		if (depthBufferingEnabled)
		{
			const renderer::RenderAttachment& depthAttachment = attachments.at(renderPassConfig.depthStencilAttachmentHandle);
			
			VkAttachmentDescription depthAttachmentDescription{};
			depthAttachmentDescription.format = depthFormat;
			depthAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(depthAttachment.sample);
			depthAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(depthAttachment.loadOp);
			depthAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(depthAttachment.storeOp);
			depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachmentDescription.initialLayout = static_cast<VkImageLayout>(depthAttachment.initialLayout);
			depthAttachmentDescription.finalLayout = static_cast<VkImageLayout>(depthAttachment.finalLayout);

			attDescriptions.push_back(depthAttachmentDescription);

			// Depth Attachment reference
			depthAttachmentReference.attachment = static_cast<uint32_t>(attDescriptions.size() - 1);
			depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		// Pipeline type subpass is to be bound to (change for raytracing)
		subpass.colorAttachmentCount = static_cast<uint32_t>(attReferences.size());
		subpass.pColorAttachments = attReferences.data();
		subpass.pDepthStencilAttachment = depthBufferingEnabled ? &depthAttachmentReference : nullptr;

		// Need to determine when layout transitions occur using subpass dependencies
		std::vector<VkSubpassDependency> subpassDependencies;


		if (depthBufferingEnabled)
		{
			subpassDependencies.resize(2);
			// Transition must happen after ...
			subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;			// Meaning outside of renderpass
			subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// pipeline stage
			subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;	// memory access
			// Transition must happen before ...
			subpassDependencies[0].dstSubpass = 0;
			subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


			//Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			// Transition must happen after ...
			subpassDependencies[1].srcSubpass = 0;
			subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// pipeline stage
			subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// memory access

			// Transition must happen before ...
			subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}
		else
		{
			subpassDependencies.resize(1);
			subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[0].dstSubpass = 0;
			subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].srcAccessMask = 0;
			subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		// Create Info for renderpass
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attDescriptions.size());
		renderPassCreateInfo.pAttachments = attDescriptions.data();

		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		VkResult result = vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &vkRenderPass_);
		assert(result == VK_SUCCESS);
	}

	void VulkanRenderPass::cleanup(VkDevice logicalDevice)
	{
		vkDestroyRenderPass(logicalDevice, vkRenderPass_, nullptr);
	}

}
