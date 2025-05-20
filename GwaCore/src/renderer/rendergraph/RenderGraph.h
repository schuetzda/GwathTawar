#pragma once
#include <concepts>
#include <map>
#include "RenderValues.h"
#include <iostream>
namespace gwa::renderer
{
	template <typename T>
	concept EnumType = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

	constexpr inline size_t INVALID_ATTACHMENT_ID = std::numeric_limits<size_t>::max();
	
	struct Attachment
	{
		Format format;
		AttachmentLoadOp loadOp;
		AttachmentStoreOp storeOp;
		SampleCountFlagBits sample;
		ImageLayout initialLayout;
		ImageLayout finalLayout;
	};
	
	struct Pass
	{
		size_t* colorAttachments;	
		size_t colorAttachmentsCount;
		size_t depthStencilAttachment;
	};

	struct RenderGraphDescription
	{
		std::map<size_t, Attachment> attachments;
		std::map<size_t, Pass> passes;
	};

	template<EnumType RenderPassRessourceIDs>
	class RenderGraph
	{
	public:
		
		RenderGraph& addAttachment(RenderPassRessourceIDs attachmentId, Format format, AttachmentLoadOp loadOp, AttachmentStoreOp storeOp, SampleCountFlagBits sample, ImageLayout initialLayout, ImageLayout finalLayout)
		{
			graphDescription.attachments.try_emplace(hash(attachmentId),
				format, loadOp, storeOp, sample, initialLayout, finalLayout);
			return *this;
		}

		template<size_t attachmentsCount>
		RenderGraph& addRenderPass(RenderPassRessourceIDs renderPassId, const std::array<RenderPassRessourceIDs, attachmentsCount>& attachmentsIds, RenderPassRessourceIDs depthAttachmentId)
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			addAttachmentToRenderpass<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.passes.try_emplace(hash(renderPassId), attachmentsHashed.data(), attachmentsHashed.size(), hash(depthAttachmentId));
			return *this;
		}

		template<size_t attachmentsCount>
		RenderGraph& addRenderPass(RenderPassRessourceIDs renderPassId, const std::array<RenderPassRessourceIDs, attachmentsCount>& attachmentsIds)
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			addAttachmentToRenderpass<attachmentsCount>(attachmentsHashed, attachmentsIds);
			
			graphDescription.passes.try_emplace(hash(renderPassId), attachmentsHashed.data(), attachmentsHashed.size(), INVALID_ATTACHMENT_ID);
			return *this;
		}

		const RenderGraphDescription& getRenderGraphDescription() const
		{
			return graphDescription;
		}



	private:
		template<size_t attachmentsCount>
		void addAttachmentToRenderpass(std::array<size_t, attachmentsCount>& attachmentsHashed, const std::array<RenderPassRessourceIDs, attachmentsCount>& attachmentsIds)
		{
			for (size_t i = 0; i < attachmentsCount; i++)
			{
				attachmentsHashed[i] = hash(attachmentsIds[i]);
				const bool attachmentExists = graphDescription.attachments.contains(attachmentsHashed[i]);
				assert(attachmentExists); //Make sure to add Attachments before including them in a RenderPass
			}
		}
		

		constexpr size_t hash(RenderPassRessourceIDs id) const
		{
			return static_cast<size_t>(id);
		}

		RenderGraphDescription graphDescription;
	};
}
