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

	template<EnumType AttachmentID, bool nodeAdded = false, bool setRenderPass=false, bool setPipeline = false, bool setDescriptorSet = false>
	class RenderGraph
	{
	public:
		RenderGraph() = default;
		explicit RenderGraph(RenderGraphDescription&& description)
			: graphDescription(std::move(description)) {
		}
		
		RenderGraph& addRenderAttachment(AttachmentID attachmentID, Format format, AttachmentLoadOp loadOp, AttachmentStoreOp storeOp, SampleCountFlagBits sample, ImageLayout initialLayout, ImageLayout finalLayout)
		{
			graphDescription.renderAttachments.try_emplace(hash(attachmentID),
				format, loadOp, storeOp, sample, initialLayout, finalLayout);
			return *this;
		}

		RenderGraph& addBufferAttachment(AttachmentID attachmentID, const void* data, uint32_t dataSize)
		{
			graphDescription.bufferAttachments.try_emplace(hash(attachmentID), data, dataSize);
			return *this;
		}

		RenderGraph& addResourceAttachment(AttachmentID attachmentID, ResourceAttachmentType type, ntity::ComponentHandle resourceHandle, ResourceAttachment::DataSizeInfo info = ResourceAttachment::DataSizeInfo(0))
		{
			graphDescription.resourceAttachments.try_emplace(hash(attachmentID), type, resourceHandle, info);
			return *this;
		}

		RenderGraph<AttachmentID, true, false, false, false> addGraphNode()
			requires !nodeAdded || (setRenderPass && setPipeline && setDescriptorSet)
		{
			graphDescription.graphNodes.emplace_back();
			return RenderGraph<AttachmentID, true, false, false, false>(std::move(graphDescription));
		}

		template<size_t attachmentsCount> 	
		RenderGraph<AttachmentID, nodeAdded, true, setPipeline, setDescriptorSet> addRenderPass(const std::array<AttachmentID, attachmentsCount>& attachmentsIds, AttachmentID depthAttachmentId) 
			requires nodeAdded
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			hashAttachmentHandles<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.graphNodes.back().renderPass = { std::vector<size_t>(attachmentsHashed.begin(), attachmentsHashed.end()), hash(depthAttachmentId) };
			return RenderGraph<AttachmentID, nodeAdded, true, setPipeline, setDescriptorSet>(std::move(graphDescription));
		}

		template<size_t attachmentsCount> 	
		RenderGraph<AttachmentID, nodeAdded, true, setPipeline, setDescriptorSet> addRenderPass(const std::array<AttachmentID, attachmentsCount>& attachmentsIds) 
			requires nodeAdded
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			hashAttachmentHandles<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.graphNodes.back().renderPass = { std::vector<size_t>(attachmentsHashed.begin(), attachmentsHashed.end()), INVALID_ATTACHMENT_ID };
			return RenderGraph<AttachmentID, nodeAdded, true, setPipeline, setDescriptorSet>(std::move(graphDescription));
		}

		RenderGraph<AttachmentID, nodeAdded, setRenderPass, setPipeline, true> addDescriptorSet(bool bindless = false) 
			requires nodeAdded
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.emplace_back();
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindless = bindless;
			return RenderGraph<AttachmentID, nodeAdded, setRenderPass, setPipeline, true>(std::move(graphDescription));
		}

		template<DescriptorType type>
		RenderGraph& addBinding(uint32_t bindingSlot, ShaderStageFlagBits shaderStage, AttachmentID inputAttachmentHandle, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires (type != DescriptorType::DESCRIPTOR_TYPE_INPUT_ATTACHMENT) && nodeAdded && setDescriptorSet
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, hash(inputAttachmentHandle), descriptorCount, maxDesciptorCount);
			return *this;
		}
		template<DescriptorType type>
		RenderGraph& addBinding(uint32_t bindingSlot, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires (type != DescriptorType::DESCRIPTOR_TYPE_INPUT_ATTACHMENT) && nodeAdded&& setDescriptorSet
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, descriptorCount, maxDesciptorCount);
			return *this;
		}

		template<DescriptorType type>
		RenderGraph& addBinding(AttachmentID inputAttachmentHandle, uint32_t bindingSlot, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires nodeAdded&& setDescriptorSet
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, hash(inputAttachmentHandle), descriptorCount, maxDesciptorCount);
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.back().isAttachmentReference = true;
			return *this;
		}
		
		RenderGraph& addTexturedMesh(AttachmentID inputAttachmentHandle)
			requires nodeAdded&& setDescriptorSet
		{
			graphDescription.graphNodes.back().texturedMeshHandles.push_back(hash(inputAttachmentHandle));
			return *this;
		}

		RenderGraph<AttachmentID, nodeAdded, setRenderPass, true, setDescriptorSet> addPipeline(PipelineConfig pipelineConfig)
		{
			graphDescription.graphNodes.back().pipelineConfig = std::move(pipelineConfig);
			return RenderGraph<AttachmentID, nodeAdded, setRenderPass, true, setDescriptorSet>(std::move(graphDescription));
		}

		RenderGraphDescription createRenderGraph() const
			requires nodeAdded && setRenderPass && setPipeline	
		{
			return graphDescription;
		}



	private:
		template<size_t attachmentsCount>
		void hashAttachmentHandles(std::array<size_t, attachmentsCount>& attachmentsHashed, const std::array<AttachmentID, attachmentsCount>& attachmentsIds)
		{
			for (size_t i = 0; i < attachmentsCount; i++)
			{
				attachmentsHashed[i] = hash(attachmentsIds[i]);
				const bool attachmentExists = graphDescription.renderAttachments.contains(attachmentsHashed[i]);
				assert(attachmentExists); //Make sure to add Attachments before including them in a RenderPass
			}
		}
		

		constexpr size_t hash(AttachmentID id) const
		{
			return static_cast<size_t>(id);
		}

		RenderGraphDescription graphDescription;
	};
}
