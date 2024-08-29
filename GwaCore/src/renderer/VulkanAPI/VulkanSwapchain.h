/* Swapchain
List of image buffers that the GPU draws into.
Those are presented to the display hardware 
*/
#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace gwa {
	class VulkanSwapchain {

	public:
		VulkanSwapchain(VkPhysicalDevice& vkPhysicalDevice, VkDevice& vkLogicalDevice, VkSurfaceKHR& vkSurface, int framebufferWidth, int framebufferHeight);
		~VulkanSwapchain();
		VkSwapchainKHR& getSwapchain() {
			return vkSwapchain;
		}
		VkFormat getSwapchainFormat()
		{
			return vkSwapchainImageFormat;
		}

		void cleanup(VkDevice& vkDevice);

		struct VulkanSwapchainImage {
			VkImage image;
			VkImageView imageView;
		};

	private:
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) const;
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, int framebufferWidth, int framebufferHeight) const;
		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const;
		VkImageView createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		VkSwapchainKHR vkSwapchain;

		VkFormat vkSwapchainImageFormat;
		VkExtent2D vkSwapchainExtent;
		std::vector<VulkanSwapchainImage> swapchainImages;

	};
}