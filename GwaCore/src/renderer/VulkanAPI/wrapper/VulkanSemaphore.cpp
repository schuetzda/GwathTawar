#include "VulkanSemaphore.h"
#include <cassert>
namespace gwa
{
	VulkanSemaphore::VulkanSemaphore(VkDevice logicalDevice, int MAX_FRAMES_IN_FLIGHT) : logicalDevice_(logicalDevice),maxFramesInFlight_(MAX_FRAMES_IN_FLIGHT)
	{
		semaphores_.resize(maxFramesInFlight_);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			assert(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &semaphores_[i]) == VK_SUCCESS);
		}
	}

	void VulkanSemaphore::cleanup()
	{
		for (size_t i = 0; i < maxFramesInFlight_; ++i)
		{
			vkDestroySemaphore(logicalDevice_, semaphores_[i], nullptr);
		}
	}
}
