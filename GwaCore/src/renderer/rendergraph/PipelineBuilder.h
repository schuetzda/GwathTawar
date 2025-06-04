#pragma once
#include <filesystem>
#include "RenderValues.h"
#include "RenderGraphData.h"
namespace gwa::renderer
{
		
	class PipelineBuilder
	{
	public:
		PipelineBuilder& addShaderModule(std::filesystem::path shaderPath, ShaderStageFlagBits stage)
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
			pipelineConfig.inputAssembly = PipelineConfig::InputAssembly{ topology, restartEnable };
			return *this;
		}

		PipelineBuilder& setViewport(float x, float y, float width, float height)
		{
			pipelineConfig.viewport = PipelineConfig::ViewportState{ x, y, width, height };
			return *this;
		}

		PipelineBuilder& setRasterizer(bool depthClampEnabled, bool rasterizerDiscard, PolygonMode polygonMode, float lineWidth, CullModeFlagBits cullMode, bool depthBiasEnable)
		{
			pipelineConfig.rasterizerConfig = PipelineConfig::RasterizerConfig{ depthClampEnabled, rasterizerDiscard, polygonMode, lineWidth, cullMode, depthBiasEnable };
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
			PipelineConfig configToReturn = std::move(pipelineConfig);
			reset();
			return configToReturn;
		}

	private:
		void reset()
		{
			pipelineConfig = PipelineConfig();
		}
		PipelineConfig pipelineConfig{};
	};
}
