#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace gwa
{
	class VulkanFence
	{
	public:
		VulkanFence() = default;
		VulkanFence(VkDevice logicalDevice, int maxFramesInFlight);
		void cleanup();
		
		const std::vector<VkFence>& getFences() const
		{
			return fences_;
		}
	private:
		std::vector<VkFence> fences_{};
		VkDevice logicalDevice_{};
		int maxFramesInFlight_{};
	};
}
