#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa {
	class VulkanPushConstant
	{
	public:
		VulkanPushConstant() = default;

		void init(VkShaderStageFlags vkShaderStageFlags, uint32_t offset, uint32_t size);

		VkPushConstantRange getRange() const
		{
			return vkPushConstantRange_;
		}
	private:
		VkPushConstantRange vkPushConstantRange_;
	};
}
