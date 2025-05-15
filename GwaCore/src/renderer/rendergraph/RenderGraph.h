#pragma once
#include <concepts>
#include <map>
#include "RenderValues.h"
#include <iostream>
namespace gwa::renderer
{
	template <typename T>
	concept EnumType = std::is_enum_v<T> && !std::is_convertible_v<T, int>;


	struct Attachment
	{
		Format format;
		AttachmentLoadOp loadOp;
		AttachmentStoreOp storeOp;
		SampleCountFlagBits sample;
	};
	
	struct Pass
	{
		size_t* attachments;	
		size_t attachmentsCount;
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
		
		RenderGraph& addAttachment(RenderPassRessourceIDs attachmentId, Format format, AttachmentLoadOp loadOp, AttachmentStoreOp storeOp, SampleCountFlagBits sample)
		{
			graphDescription.attachments.try_emplace(hash(attachmentId),
				format, loadOp, storeOp, sample);
			return *this;
		}

		template<size_t attachmentsCount>
		RenderGraph& addRenderPass(RenderPassRessourceIDs renderPassId, const std::array<RenderPassRessourceIDs, attachmentsCount>& attachmentsIds)
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			for (size_t i=0; i < attachmentsCount; i++)
			{
				attachmentsHashed[i] = hash(attachmentsIds[i]);
				const bool attachmentExists = graphDescription.attachments.contains(attachmentsHashed[i]);
				assert(attachmentExists); //Make sure to add Attachments before you include them in a RenderPass
			}
			graphDescription.passes.try_emplace(hash(renderPassId),attachmentsHashed.data(), attachmentsHashed.size());
			return *this;
		}

		const RenderGraphDescription& getRenderGraphDescription() const
		{
			return graphDescription;
		}



	private:
		constexpr size_t hash(RenderPassRessourceIDs id) const
		{
			return static_cast<size_t>(id);
		}

		RenderGraphDescription graphDescription;
	};
}
