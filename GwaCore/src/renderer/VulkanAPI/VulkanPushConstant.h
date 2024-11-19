#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa {
	class VulkanPushConstant
	{
	public:
		VulkanPushConstant(VkShaderStageFlags, uint32_t offset, uint32_t size);
		VkPushConstantRange& getPushConstantRange() {
			return vkPushConstantRange;
		}
		void cleanup();
	private:
		VkPushConstantRange vkPushConstantRange;
	};
}
