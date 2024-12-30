#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa
{
	class VulkanSemaphore
	{
	public:
		VulkanSemaphore() = default;
		VulkanSemaphore(VkDevice logicalDevice, int maxFramesInFlight);

		void cleanup();
		
		const std::vector<VkSemaphore>& getSemaphores() const
		{
			return semaphores_;
		}
	private:
		std::vector<VkSemaphore> semaphores_;
		VkDevice logicalDevice_;
		int maxFramesInFlight_;
	};
}
