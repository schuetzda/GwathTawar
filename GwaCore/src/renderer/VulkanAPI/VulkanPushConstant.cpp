#include "VulkanPushConstant.h"
namespace gwa {
	VulkanPushConstant::VulkanPushConstant(VkShaderStageFlags vkShaderStageFlags, uint32_t offset, uint32_t size)
	{
		vkPushConstantRange.stageFlags = vkShaderStageFlags;//Where the push constant goes to
		vkPushConstantRange.offset = offset;								// Offset into given datta to pass to push constant
		vkPushConstantRange.size = size;						// Size of data being passed
	}
}
