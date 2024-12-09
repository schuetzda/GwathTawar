#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace gwa
{
	class VulkanFence
	{
	public:
		VulkanFence(VkDevice logicalDevice, const int MAX_FRAMES_IN_FLIGHT);
		const std::vector<VkFence>& getFences() const
		{
			return fences;
		}
		void cleanup(VkDevice logicalDevice);
	private:
		std::vector<VkFence> fences;
		const int MAX_FRAMES_IN_FLIGHT;
	};
}
