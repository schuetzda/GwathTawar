#pragma once
#include <vector>
#include "RenderValues.h"
#include <cassert>

namespace gwa::renderer
{
		struct DescriptorBindingConfig
		{
			DescriptorType type{};
			uint32_t bindingSlot{};
			ShaderStageFlagBits shaderStage{};
		};

		struct DescriptorSetConfig
		{
			std::vector<DescriptorBindingConfig> bindings{};
			uint32_t descriptorCount{ 1 };
			bool bindless{};
		};
	class DescriptorSetConfigurator
	{
	public:

		DescriptorSetConfigurator() = default;
		DescriptorSetConfigurator& addBinding(uint32_t bindingSlot, DescriptorType type, ShaderStage shaderStage)
		{
			currentDescriptorSet.bindings.emplace_back(type, bindingSlot, shaderStage);
			return *this;
		}

		DescriptorSetConfigurator& finalizeDescriptorSet(bool bindless = false, uint32_t descriptorCount = 1)
		{
			assert(!currentDescriptorSet.bindings.empty()); //Make sure to add bindings before you create a descriptor set

			currentDescriptorSet.descriptorCount = descriptorCount;
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
