#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanSemaphore
	{
	public:
		VulkanSemaphore(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT);

		const std::vector<VkSemaphore>& getSemaphores () const
		{
			return vkSemaphores;
		}

		void cleanup(VkDevice logicalDevice);

	private:
		std::vector<VkSemaphore> vkSemaphores;
		const int MAX_FRAMES_IN_FLIGHT;
	};
}
