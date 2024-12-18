#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace gwa
{
	class VulkanFence
	{
	public:
		VulkanFence(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT);
		void cleanup(VkDevice logicalDevice);

		std::vector<VkFence> fences;
	private:
		const int MAX_FRAMES_IN_FLIGHT;
	};
}
