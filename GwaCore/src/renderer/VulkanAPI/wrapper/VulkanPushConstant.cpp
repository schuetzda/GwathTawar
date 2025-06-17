#include "VulkanPushConstant.h"
namespace gwa {
	VulkanPushConstant::VulkanPushConstant(VkShaderStageFlags vkShaderStageFlags, uint32_t offset, uint32_t size)
	{
		vkPushConstantRange_.stageFlags = vkShaderStageFlags;	//Where the push constant goes to
		vkPushConstantRange_.offset = offset;					// Offset into given data to pass to push constant
		vkPushConstantRange_.size = size;						// Size of data being passed
	}
}
