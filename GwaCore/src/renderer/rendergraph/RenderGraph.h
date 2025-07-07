#pragma once
#include <concepts>
#include <map>
#include "RenderValues.h"
#include <iostream>
#include "RenderGraphData.h"

namespace gwa::renderer
{
	/**
	 * @brief Constrain AttachmentID types to enum classes.
	*/
	template <typename T>
	concept EnumType =
		std::is_enum_v<T> &&                                 // Type has to be an enum
		!std::is_convertible_v<T, int>&&                     // It is an enum type since there is no implicit convertion to int
		std::is_integral_v<std::underlying_type_t<T>>;       // Underlying type can be cast to size_t safely (since it is an integral)

	constexpr inline size_t INVALID_ATTACHMENT_ID = std::numeric_limits<size_t>::max();

	/**
	 * @brief A fluent-interface-based system to define a rendering graph consisting of render passes, attachments, pipelines, and descriptor sets.
	 *
	 * @tparam AttachmentID Enum type used to identify attachments uniquely and reference them between different render passes.
	 * @tparam isInitialized Has the graph been initialized
	 * @tparam nodeAdded Has the graph a render node.
	 * @tparam setRenderPass Has the graph a render pass
	 * @tparam setPipeline Has the graph a set pipeline
	 * @tparam setDescriptorSet Was a descriptor set added
	 * @tparam isFinalized Is the graph finalized and build
	 */
	template<EnumType AttachmentID, bool isInitialized = false, bool nodeAdded = false, bool setRenderPass=false, bool setPipeline = false, bool setDescriptorSet = false, bool isFinalized = false>
	class RenderGraph
	{
	public:
		RenderGraph() = default;
		explicit RenderGraph(RenderGraphDescription&& description)
			: graphDescription(std::move(description)) {
		}

		/**
		 * @brief Initializes the render graph and resets the graph description. Previous stored information will be lost. Init has to be called at the beginning of creating a new render graph.
		 *
		 * @return A new instance of RenderGraph marked as initialized.
		 */
		RenderGraph<AttachmentID, true, false, false, false, false> init()
		{
			graphDescription = RenderGraphDescription();
			return RenderGraph<AttachmentID, true, false, false, false, false>(std::move(graphDescription));
		}
		
		/**
		 * @brief Adds a render attachment to the graph. This attachment has to be linked to the render pass when f.e. calling addRenderpass.
		 *
		 * @param attachmentID ID of the attachment.
		 * @param format Format of the attachment.
		 * @param finalLayout Image layout after being rendered.
		 * @param initialLayout Image layout at beginning of a render pass.
		 * @param loadOp Load operation.
		 * @param storeOp Store operation.
		 * @param sample Number of samples.
		 * @return Reference to the current RenderGraph.
		 * 
		 * @requires The render graph must be initialized.
		 */
		RenderGraph& addRenderAttachment(AttachmentID attachmentID, Format format, ImageLayout finalLayout= ImageLayout::IMAGE_LAYOUT_PRESENT_SRC_KHR, ImageLayout initialLayout = ImageLayout::IMAGE_LAYOUT_UNDEFINED, AttachmentLoadOp loadOp = AttachmentLoadOp::ATTACHMENT_LOAD_OP_CLEAR, AttachmentStoreOp storeOp = AttachmentStoreOp::ATTACHMENT_STORE_OP_STORE,
			SampleCountFlagBits sample = SampleCountFlagBits::SAMPLE_COUNT_1_BIT)
			requires isInitialized
		{
			graphDescription.renderAttachments.try_emplace(hash(attachmentID),
				format, loadOp, storeOp, sample, initialLayout, finalLayout);
			return *this;
		}

		/**
		 * @brief Adds a resource attachment to the render graph.
		 *
		 * @param attachmentID Unique identifier for the attachment.
		 * @param type Type of the resource (f.e., TEXTURED_MESH, UNIFORM_BUFFER).
		 * @param resourceHandle Handle to the ECS component representing the resource.
		 * @param info Optional size-related metadata for the resource (default is {0}).
		 *
		 * @return Reference to the current RenderGraph instance for chaining.
		 *
		 * @requires The render graph must be initialized.
		 */
		RenderGraph& addResourceAttachment(AttachmentID attachmentID, ResourceAttachmentType type, ntity::ComponentHandle resourceHandle, ResourceAttachment::DataSizeInfo info = ResourceAttachment::DataSizeInfo{0})
			requires isInitialized
		{
			graphDescription.resourceAttachments.try_emplace(hash(attachmentID), type, resourceHandle, info);
			return *this;
		}

		/**
		 * @brief Adds a new render graph node to the render graph description.
		 *
		 * @tparam AttachmentID   The type used to uniquely identify resource attachments.
		 *
		 * @return A new RenderGraph instance representing the next pipeline stage of construction,
		 *         with compile-time flags updated (e.g., `nodeAdded` set to `true`).
		 *
		 * @note This function is only available when the render graph is initialized
		 *       and the previous node (if any) has had its required components set
		 *       (render pass, pipeline, descriptor set).
		 *
		 * @requires The render graph must be initialized.
		 * @requires If a previous node has already been added, that node must have set the renderpass, pipeline and a descriptor set.
		 */
		RenderGraph<AttachmentID, true, true, false, false, false, false> addGraphNode()
			requires isInitialized && (!nodeAdded || (setRenderPass && setPipeline && setDescriptorSet))
		{
			graphDescription.graphNodes.emplace_back();
			return RenderGraph<AttachmentID,true, true, false, false, false, false>(std::move(graphDescription));
		}

		/**
		 * @brief Adds a render pass description to the current render graph node.
		 *
		 * @tparam attachmentsCount The number of color attachments.
		 * @param attachmentsIds An array of color attachment IDs used in this render pass.
		 * @param depthAttachmentId The ID of the depth attachment.
		 * @param renderFullscreenPass Whether the render pass should execute a fullscreen pass (default: false).
		 *
		 * @return A new RenderGraph instance representing the updated builder state	
		 *
		 * @note This function can only be called after a render graph node has been added
		 *       and the graph has been initialized.
		 *
		 * @requires To call this function the render graph must be initialized and a render graph node has to been added.	
		 *
		 * @see addGraphNode() to begin a new render graph node.
		 */
		template<size_t attachmentsCount> 	
		RenderGraph<AttachmentID, true, nodeAdded, true, setPipeline, setDescriptorSet, false> addRenderPass(const std::array<AttachmentID, attachmentsCount>& attachmentsIds, AttachmentID depthAttachmentId, bool renderFullscreenPass = false) 
			requires nodeAdded && isInitialized
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			hashAttachmentHandles<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.graphNodes.back().renderPass = { std::vector<size_t>(attachmentsHashed.begin(), attachmentsHashed.end()), hash(depthAttachmentId), renderFullscreenPass };
			return RenderGraph<AttachmentID, true, nodeAdded, true, setPipeline, setDescriptorSet, false>(std::move(graphDescription));
		}

		/**
		 * @brief Adds a render pass description without a depth attachment to the current render graph node.
		 *
		 * @tparam attachmentsCount The number of color attachments.
		 * @param attachmentsIds An array of color attachment IDs used in this render pass.
		 * @param renderFullscreenPass Whether the render pass should execute a fullscreen pass (default: false).
		 *
		 * @return A new RenderGraph instance representing the updated builder state
		 *
		 * @requires To call this function the render graph must be initialized and a render graph node has to been added.
		 */
		template<size_t attachmentsCount> 	
		RenderGraph<AttachmentID, true, nodeAdded, true, setPipeline, setDescriptorSet, false> addRenderPass(const std::array<AttachmentID, attachmentsCount>& attachmentsIds, bool renderFullscreenPass = false)
			requires nodeAdded && isInitialized
		{
			std::array<size_t, attachmentsCount> attachmentsHashed;
			hashAttachmentHandles<attachmentsCount>(attachmentsHashed, attachmentsIds);

			graphDescription.graphNodes.back().renderPass = { std::vector<size_t>(attachmentsHashed.begin(), attachmentsHashed.end()), INVALID_ATTACHMENT_ID, renderFullscreenPass };
			return RenderGraph<AttachmentID, true, nodeAdded, true, setPipeline, setDescriptorSet, false>(std::move(graphDescription));
		}

		/**
		 * @brief Adds a descriptor set configuration to the current render graph node.
		 *
		 * This function appends a descriptor set config to the latest render graph node,
		 * optionally enabling bindless support. Bindless descriptor sets allow shaders to access
		 * a large number of resources dynamically using indices, which is useful for techniques like
		 * material indexing or texture arrays.
		 *
		 * @param bindless Whether the descriptor set should use bindless binding (default: false).
		 *
		 * @return A new RenderGraph instance representing the updated builder state
		 *
		 * @requires To call this function the render graph must be initialized and a render graph node has to been added.
		 */
		RenderGraph<AttachmentID,true, nodeAdded, setRenderPass, setPipeline, true, false> addDescriptorSet(bool bindless = false) 
			requires nodeAdded && isInitialized
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.emplace_back();
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindless = bindless;
			return RenderGraph<AttachmentID, true, nodeAdded, setRenderPass, setPipeline, true, false>(std::move(graphDescription));
		}

		/**
		 * @brief Adds a non-image-sampler binding to the most recently added descriptor set in the current render graph node.
		 *
		 * @tparam type The descriptor type (must not be DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER).
		 * @param inputAttachmentHandle The logical handle identifying the input resource to bind.
		 * @param bindingSlot The binding slot number in the descriptor set layout.
		 * @param shaderStage The shader stage(s) this binding will be accessible from.
		 * @param descriptorCount The number of descriptors to bind (default is 1).
		 * @param maxDesciptorCount The maximum number of descriptors if this is a partially bound array or bindless binding (default is 1).
		 *
		 * @return A reference to the current RenderGraph object.
		 *
		 * @requires To call this function the render graph must be initialized, a render graph node has to been added and a descriptor set been set.
		 */
		template<DescriptorType type>
		RenderGraph& addBinding(AttachmentID inputAttachmentHandle, uint32_t bindingSlot, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires (type != DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) && nodeAdded && setDescriptorSet && isInitialized
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, hash(inputAttachmentHandle), descriptorCount, maxDesciptorCount);
			return *this;
		}

		/**
		 * @brief Adds a descriptor binding (e.g., sampler, uniform buffer, etc.) to the most recently added descriptor set.
		 *
		 * This overload is used for descriptor types that do not require an input attachment handle,
		 * such as samplers or standalone buffers.
		 *
		 * @tparam type The descriptor type to bind (e.g., DESCRIPTOR_TYPE_SAMPLER, UNIFORM_BUFFER, etc.).
		 * @param bindingSlot The binding slot number in the descriptor set layout.
		 * @param shaderStage The shader stage(s) this descriptor will be accessible from.
		 * @param descriptorCount The number of descriptors to bind (default is 1).
		 * @param maxDesciptorCount The maximum number of descriptors if this is used with bindless or arrays (default is 1).
		 *
		 * @return A reference to the current RenderGraph object.
		 *
		 * @requires To call this function the render graph must be initialized, a render graph node has to been added and a descriptor set been set.
		 */
		template<DescriptorType type>
		RenderGraph& addBinding(uint32_t bindingSlot, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires nodeAdded&& setDescriptorSet && isInitialized
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, descriptorCount, maxDesciptorCount);
			return *this;
		}

		/**
		 * @brief Adds a combined image sampler binding to the most recently added descriptor set.
		 *
		 * This function is specialized for descriptors of type `DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER` and is used
		 * to bind image attachments (e.g., textures) as inputs to shaders.
		 *
		 * @tparam type The descriptor type (must be `DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`).
		 * @param inputAttachmentHandle The ID of the input attachment resource (typically a texture or image).
		 * @param bindingSlot The binding slot number in the descriptor set layout.
		 * @param shaderStage The shader stage(s) this descriptor will be visible in.
		 * @param descriptorCount The number of descriptors to bind (default is 1).
		 * @param maxDesciptorCount The maximum number of descriptors, useful for bindless or arrayed descriptors (default is 1).
		 *
		 * @return A reference to the current RenderGraph object.
		 *
		 * @note This function automatically flags the binding as an attachment reference.
		 *
		 * @requires The render graph must be initialized, a node must have been added, and a descriptor set must be active.
		 * Only available when `type == DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`.
		 */
		template<DescriptorType type>
		RenderGraph& addBinding(AttachmentID inputAttachmentHandle, uint32_t bindingSlot, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
			requires nodeAdded&& setDescriptorSet && isInitialized && (type == DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.emplace_back(type, bindingSlot, shaderStage, hash(inputAttachmentHandle), descriptorCount, maxDesciptorCount);
			graphDescription.graphNodes.back().descriptorSetConfigs.back().bindings.back().isAttachmentReference = true;
			return *this;
		}
		
		/**
		 * @brief Adds a textured mesh resource to the current render graph node.
		 *
		 * @param inputAttachmentHandle The identifier of the textured mesh resource attachment.
		 *
		 * @return A reference to the current RenderGraph object.
		 *
		 * @requires The render graph must be initialized, a node must have been added, and a descriptor set must be active.
		 */
		RenderGraph& addTexturedMesh(AttachmentID inputAttachmentHandle)
			requires nodeAdded&& setDescriptorSet && isInitialized
		{
			graphDescription.graphNodes.back().texturedMeshHandles.push_back(hash(inputAttachmentHandle));
			return *this;
		}

		/**
		 * @brief Assigns a pipeline configuration to the current render graph node.
		 *
		 * @param pipelineConfig The pipeline configuration to apply to the render node.
		 *
		 * @return A reference to the current RenderGraph object.
		 *
		 * @note .For PipelineConfig use the PipelineBuilder
		 *
		 * @requires The render graph must be initialized.
		 */
		RenderGraph<AttachmentID, true, nodeAdded, setRenderPass, true, setDescriptorSet, false> addPipeline(PipelineConfig pipelineConfig)
			requires isInitialized
		{
			graphDescription.graphNodes.back().pipelineConfig = std::move(pipelineConfig);
			return RenderGraph<AttachmentID, true, nodeAdded, setRenderPass, true, setDescriptorSet, false>(std::move(graphDescription));
		}

		RenderGraph<AttachmentID, false, nodeAdded, setRenderPass, true, setDescriptorSet, true> createRenderGraph()
			requires nodeAdded && setRenderPass && setPipeline && isInitialized
		{
			return RenderGraph<AttachmentID, false, nodeAdded, setRenderPass, true, setDescriptorSet, true>(std::move(graphDescription));
		}

		/**
		 * @brief Finalizes and returns a complete render graph.
		 *
		 * This function marks the end of the render graph construction phase and returns
		 * a fully configured RenderGraph object that can be used to build rendering passes.
		 *
		 * @return A completed RenderGraphDescription instance with all required components (render pass, pipeline, descriptor sets) that were previously defined.
		 *
		 * @requires The render graph must be initialized, a node must have been added, a pipeline must been set and a descriptor set must be active.
		 *
		 * @see addGraphNode()
		 * @see addRenderPass()
		 * @see addPipeline()
		 * @see addDescriptorSet()
		 */
		RenderGraphDescription getRenderGraphDescription() const
			requires isFinalized
		{
			return graphDescription;
		}

	private:
		/**
		 * @brief Hashes a list of attachment IDs and verifies their existence in the render graph.
		 *
		 * @tparam attachmentsCount The number of attachments being hashed.
		 * @param[out] attachmentsHashed The output array to store hashed attachment handles.
		 * @param[in] attachmentsIds The input array of `AttachmentID`s to be hashed.
		 *
		 * @throws Assertion failure if any `AttachmentID` does not exist in `graphDescription.renderAttachments`.
		 */
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
		
		/**
		 * @brief Computes a hash value for a given AttachmentID.
		 *
		 * @param id The attachment ID to be hashed.
		 * @return A size_t hash value corresponding to the given attachment ID.
		 */
		constexpr size_t hash(AttachmentID id) const
		{
			return static_cast<size_t>(id);
		}

		RenderGraphDescription graphDescription;
	};
}
