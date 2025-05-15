#pragma once
#include <concepts>
#include <map>
#include "RenderValues.h"
namespace gwa::renderer
{
	template <typename T>
	concept EnumType = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

	template<EnumType RenderPassId, EnumType AttachmentId>
	class RenderGraph
	{
	public:
		struct RenderAttachment
		{
			Format format;
			AttachmentLoadOp loadOp;
			AttachmentStoreOp storeOp;
			uint32_t samples;
		};
		struct RenderPass
		{
			RenderPassId renderPassId;
		};

		void addAttachment(AttachmentId id, RenderAttachment&& attachment)
		{
			attachments.emplace(id, std::move<RenderAttachment>(attachment));
		}

		void addRenderPass(RenderPassId id, RenderPass&& renderPass)
		{
			passes.emplace(id, std::move<RenderPass>(renderPass));
		}

	private:
		std::map<AttachmentId, RenderAttachment> attachments;
		std::map<RenderPassId, RenderPass> passes;
	};
}
