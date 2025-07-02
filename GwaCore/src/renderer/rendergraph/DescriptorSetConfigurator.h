#pragma once
#include <vector>
#include "RenderValues.h"
#include "RenderGraphData.h"
#include <cassert>

namespace gwa::renderer
{
	class DescriptorSetConfigurator
	{
	public:

		DescriptorSetConfigurator() = default;
		DescriptorSetConfigurator& addBinding(uint32_t bindingSlot, DescriptorType type, ShaderStageFlagBits shaderStage, uint32_t descriptorCount = 1, uint32_t maxDesciptorCount = 1)
		{
			currentDescriptorSet.bindings.emplace_back(type, bindingSlot, shaderStage, descriptorCount, maxDesciptorCount);
			return *this;
		}

		DescriptorSetConfigurator& finalizeDescriptorSet(bool bindless = false)
		{
			assert(!currentDescriptorSet.bindings.empty()); //Make sure to add bindings before you create a descriptor set
			currentDescriptorSet.bindless = bindless;

			descriptorSetsConfig.emplace_back(std::move(currentDescriptorSet));
			currentDescriptorSet = DescriptorSetConfig();

			return *this;
		}
		const std::vector<DescriptorSetConfig>& getDescriptorSets() const
		{
			return descriptorSetsConfig;
		}

	private:
		std::vector<DescriptorSetConfig> descriptorSetsConfig;
		DescriptorSetConfig currentDescriptorSet;
	};
}
