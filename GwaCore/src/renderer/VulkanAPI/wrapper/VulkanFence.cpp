#include "VulkanFence.h"
#include <cassert>
namespace gwa
{
	VulkanFence::VulkanFence(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT):maxFramesInFlight_(MAX_FRAMES_IN_FLIGHT), logicalDevice_(logicalDevice)
	{
		fences_.resize(MAX_FRAMES_IN_FLIGHT);

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			assert(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fences_[i]) == VK_SUCCESS);
		}
	}
	void VulkanFence::cleanup()
	{
		for (size_t i = 0; i < maxFramesInFlight_; ++i)
		{
			vkDestroyFence(logicalDevice_, fences_[i], nullptr);
		}
	}
}
