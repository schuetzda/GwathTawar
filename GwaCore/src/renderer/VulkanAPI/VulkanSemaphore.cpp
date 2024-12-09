#include "VulkanSemaphore.h"
#include <cassert>
namespace gwa
{
	VulkanSemaphore::VulkanSemaphore(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT) : MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT)
	{
		vkSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			assert(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &vkSemaphores[i]) == VK_SUCCESS);
		}
	}

	void VulkanSemaphore::cleanup(VkDevice logicalDevice)
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(logicalDevice, vkSemaphores[i], nullptr);
		}
	}
}
