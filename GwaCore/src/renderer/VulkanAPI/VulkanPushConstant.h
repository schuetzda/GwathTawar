#pragma once
#include <vulkan/vulkan_core.h>
namespace gwa {
	class VulkanPushConstant
	{
	public:
		VulkanPushConstant(VkShaderStageFlags vkShaderStageFlags, uint32_t offset, uint32_t size);
		VkPushConstantRange getPushConstantRange() const{
			return vkPushConstantRange;
		}
		void cleanup();
	private:
		VkPushConstantRange vkPushConstantRange;
	};
}
