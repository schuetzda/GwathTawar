#pragma once
#include <filesystem>
#include "RenderValues.h"
namespace gwa::renderer
{
	struct PipelineConfig
	{
		std::vector<ShaderModuleConfig> shaderModules;
		std::vector<VertexInput> vertexInputs;
		InputAssembly inputAssembly;
		ViewportState viewport;
		bool enableMSAA = false;
		bool enableDepthTesting = true;
	};
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
		ShaderStage stage;
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

	class PipelineBuilder
	{
	public:
		PipelineBuilder& addShaderModule(std::filesystem::path shaderPath, ShaderStage stage)
		{
			pipelineConfig.shaderModules.emplace_back(shaderPath, stage);
			return *this;
		}

		PipelineBuilder& addVertexInput(uint32_t binding, uint32_t stride, uint32_t location, uint32_t offset, Format format)
		{
			pipelineConfig.vertexInputs.emplace_back(binding, stride, location, offset, format);
			return *this;
		}

		PipelineBuilder& setPipelineInputAssembly(PrimitiveTopology topology, bool restartEnable)
		{
			pipelineConfig.inputAssembly = InputAssembly{ topology, restartEnable };
			return *this;
		}

		PipelineBuilder& setViewport(float x, float y, float width, float height)
		{
			pipelineConfig.viewport = ViewportState{ x, y, width, height };
			return *this;
		}

		PipelineBuilder& setMSAA(bool enable)
		{
			pipelineConfig.enableMSAA = enable;
			return *this;
		}

		PipelineBuilder& setDepthBuffering(bool enable)
		{
			pipelineConfig.enableDepthTesting = enable;
			return *this;
		}

		PipelineConfig build()
		{

		}
	private:
		PipelineConfig pipelineConfig{};
	};
}
