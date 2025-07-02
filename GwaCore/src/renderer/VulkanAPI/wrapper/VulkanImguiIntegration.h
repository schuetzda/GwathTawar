#pragma once
#include <vulkan/vulkan.h>

namespace gwa::renderer
{
	class VulkanImguiIntegration
	{
	public:
		VulkanImguiIntegration() = default;
		VulkanImguiIntegration(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkInstance instance, VkRenderPass renderpass, VkQueue graphicsQueue);

		void cleanup(VkDevice logicalDevice);
		void updatePlatform();
		void renderData(VkCommandBuffer commandBuffer) const;
	private:
		VkDescriptorPool imguiPool;
	};
 }
