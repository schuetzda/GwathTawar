#pragma once
#include "RenderValues.h"
#include <map>
#include <filesystem>

namespace gwa::renderer
{
	struct RenderPassConfig
	{
		size_t* outputAttachmentHandles;
		uint32_t outputAttachmentsCount;
		size_t depthStencilAttachmentHandle;
	};

	struct PipelineConfig
	{
		struct VertexInput
		{
			uint32_t binding;
			uint32_t stride;
			uint32_t location;
			uint32_t offset;
			Format format;
		};

		struct InputAssembly
		{
			PrimitiveTopology topology = PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			bool restartEnable = false;
		};
		struct ShaderModuleConfig
		{
			std::filesystem::path shaderPath;
			ShaderStageFlagBits stage;
		};

		struct RasterizerConfig
		{
			bool depthClampEnabled = false;
			bool rasterizerDiscard = false;
			PolygonMode polygonMode = PolygonMode::POLYGON_MODE_FILL;
			float lineWidth = 1.f;
			CullModeFlagBits cullMode = CullModeFlagBits::CULL_MODE_NONE;
			bool depthBiasEnable = false;
		};

		struct ViewportState
		{
			float x;
			float y;
			float width;
			float height;
			//scissor not included yet
		};

		std::vector<ShaderModuleConfig> shaderModules{};
		std::vector<VertexInput> vertexInputs{};
		InputAssembly inputAssembly{};
		ViewportState viewport{};
		RasterizerConfig rasterizerConfig{};
		bool enableMSAA = false;
		bool enableDepthTesting = true;
	};

	struct DescriptorBindingConfig
	{
		DescriptorType type{};
		uint32_t bindingSlot{};
		ShaderStageFlagBits shaderStage{};
		uint32_t descriptorCount{ 1 };
		uint32_t maxDescriptorCount{ 1 };
		size_t inputAttachmentHandle{ 0 };
		DescriptorBindingConfig(DescriptorType t, uint32_t slot, ShaderStageFlagBits stage, uint32_t count = 1, uint32_t maxCount = 1)
			: type(t), bindingSlot(slot), shaderStage(stage), descriptorCount(count), maxDescriptorCount(maxCount) {
		}
		DescriptorBindingConfig(DescriptorType t, uint32_t slot, ShaderStageFlagBits stage, size_t inputAttachmentHandle, uint32_t count = 1, uint32_t maxCount = 1)
			: type(t), bindingSlot(slot), shaderStage(stage), descriptorCount(count), maxDescriptorCount(maxCount), inputAttachmentHandle(inputAttachmentHandle) {
		}
	};

	struct DescriptorSetConfig
	{
		std::vector<DescriptorBindingConfig> bindings{};
		bool bindless{};
	};

	struct RenderGraphNode
	{
		RenderPassConfig renderPass{};
		PipelineConfig pipelineConfig{};
		std::vector<DescriptorSetConfig> descriptorSetConfigs{};
	};
	struct Attachment
	{
		Format format;
		AttachmentLoadOp loadOp;
		AttachmentStoreOp storeOp;
		SampleCountFlagBits sample;
		ImageLayout initialLayout;
		ImageLayout finalLayout;
	};


	struct RenderGraphDescription
	{
		std::map<size_t, Attachment> attachments{};
		std::vector<RenderGraphNode> graphNodes{};
	};



}
