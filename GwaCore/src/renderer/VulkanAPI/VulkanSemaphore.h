#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanSemaphore
	{
	public:
		VulkanSemaphore(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT);

		void cleanup(VkDevice logicalDevice);

		std::vector<VkSemaphore> vkSemaphores;
	private:
		const int MAX_FRAMES_IN_FLIGHT;
	};
}
