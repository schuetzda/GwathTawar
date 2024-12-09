#include "VulkanFence.h"
#include <cassert>
namespace gwa
{
	VulkanFence::VulkanFence(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT): MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT)
	{
		fences.resize(MAX_FRAMES_IN_FLIGHT);

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			assert(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fences[i]) == VK_SUCCESS);
		}
	}
	void VulkanFence::cleanup(VkDevice logicalDevice)
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroyFence(logicalDevice, fences[i], nullptr);
		}
	}
}
