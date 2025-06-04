#pragma once
#include <concepts>
#include <map>
#include "RenderValues.h"
#include <iostream>
#include "RenderGraphData.h"

namespace gwa::renderer
{
	template <typename T>
	concept EnumType = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

	constexpr inline size_t INVALID_ATTACHMENT_ID = std::numeric_limits<size_t>::max();

	template<EnumType AttachmentsIDs, bool nodeAdded = false, bool setRenderPass=false, bool setPipeline = false, bool setDescriptorSet = false>
	class RenderGraph
	{
	public:
		RenderGraph() = default;
		explicit RenderGraph(RenderGraphDescription&& description)
			: graphDescription(std::move(description)) {
		}
		
		RenderGraph& addAttachment(AttachmentsIDs attachmentId, Format format, AttachmentLoadOp loadOp, AttachmentStoreOp storeOp, SampleCountFlagBits sample, ImageLayout initialLayout, ImageLayout finalLayout)
		{
			graphDescription.attachments.try_emplace(hash(attachmentId),
				format, loadOp, storeOp, sample, initialLayout, finalLayout);
			return *this;
		}

		RenderGraph<AttachmentsIDs, true, false, false, false> addGraphNode()
		{
			graphDescription.graphNodes.emplace_back();
			return RenderGraph<AttachmentsIDs, true, false, false, false>(std::move(graphDescription));
		}

		template<size_t attachmentsCount> 	
		RenderGraph<AttachmentsIDs, nodeAdded, true, setPipeline, setDescriptorSet> addRenderPass(const std::array<AttachmentsIDs, attachmentsCount>& attachmentsIds, AttachmentsIDs depthAttachmentId) 
			requires nodeAdded
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			hashAttachmentHandles<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.graphNodes.back().renderPass = { attachmentsHashed.data(), static_cast<uint32_t>(attachmentsHashed.size()), hash(depthAttachmentId) };
			return RenderGraph<AttachmentsIDs, nodeAdded, true, setPipeline, setDescriptorSet>(std::move(graphDescription));
		}

		template<size_t attachmentsCount> 	
		RenderGraph<AttachmentsIDs, nodeAdded, true, setPipeline, setDescriptorSet> addRenderPass(const std::array<AttachmentsIDs, attachmentsCount>& attachmentsIds) 
			requires nodeAdded
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			hashAttachmentHandles<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.graphNodes.back().renderPass = { attachmentsHashed.data(), attachmentsHashed.size(), INVALID_ATTACHMENT_ID };
			return RenderGraph<AttachmentsIDs, nodeAdded, true, setPipeline, setDescriptorSet>(std::move(graphDescription));
		}

		RenderGraph<AttachmentsIDs, nodeAdded, setRenderPass, setPipeline, true> addDescriptorSet(bool bindless = false) 
			requires nodeAdded
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.emplace_back();
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindless = bindless;
			return RenderGraph<AttachmentsIDs, nodeAdded, setRenderPass, setPipeline, true>(std::move(graphDescription));
		}

		template<DescriptorType type>
		RenderGraph& addBinding(uint32_t bindingSlot, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires (type != DescriptorType::DESCRIPTOR_TYPE_INPUT_ATTACHMENT) && nodeAdded && setDescriptorSet
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, descriptorCount, maxDesciptorCount);
			return *this;
		}

		template<DescriptorType type>
		RenderGraph& addBinding(uint32_t bindingSlot, ShaderStageFlagBits shaderStage, size_t inputAttachmentHandle, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires (type == DescriptorType::DESCRIPTOR_TYPE_INPUT_ATTACHMENT) && nodeAdded&& setDescriptorSet
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, inputAttachmentHandle, descriptorCount, maxDesciptorCount);
			return *this;
		}

		RenderGraph<AttachmentsIDs, nodeAdded, setRenderPass, true, setDescriptorSet> addPipeline(PipelineConfig pipelineConfig)
		{
			graphDescription.graphNodes.back().pipelineConfig = std::move(pipelineConfig);
			return RenderGraph<AttachmentsIDs, nodeAdded, setRenderPass, true, setDescriptorSet>(std::move(graphDescription));
		}

		RenderGraphDescription createRenderGraph() const
			requires nodeAdded && setRenderPass && setPipeline && setDescriptorSet
		{
			return graphDescription;
		}



	private:
		template<size_t attachmentsCount>
		void hashAttachmentHandles(std::array<size_t, attachmentsCount>& attachmentsHashed, const std::array<AttachmentsIDs, attachmentsCount>& attachmentsIds)
		{
			for (size_t i = 0; i < attachmentsCount; i++)
			{
				attachmentsHashed[i] = hash(attachmentsIds[i]);
				const bool attachmentExists = graphDescription.attachments.contains(attachmentsHashed[i]);
				assert(attachmentExists); //Make sure to add Attachments before including them in a RenderPass
			}
		}
		

		constexpr size_t hash(AttachmentsIDs id) const
		{
			return static_cast<size_t>(id);
		}

		RenderGraphDescription graphDescription;
	};
}
