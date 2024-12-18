/* Swapchain
List of image buffers that the GPU draws into.
Those are presented to the display hardware 
*/
#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace gwa {
	struct VulkanSwapchainImage {
		VkImage image;
		VkImageView imageView;
	};

	class VulkanSwapchain {

	public:
		VulkanSwapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR vkSurface, int framebufferWidth, int framebufferHeight);
		~VulkanSwapchain();

		void recreateSwapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int framebufferWidth, int framebufferHeight);
		void cleanup(VkDevice logicalDevice);

		VkSwapchainKHR vkSwapchain;
		VkFormat vkSwapchainImageFormat;
		VkExtent2D vkSwapchainExtent;
		std::vector<VulkanSwapchainImage> swapchainImages;

	private:
		void createSwapchain(VkDevice logicalDevice, VkPhysicalDevice phyiscalDevice, VkSurfaceKHR vkSurface, int framebufferWidth, int framebufferHeight);
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) const;
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, int framebufferWidth, int framebufferHeight) const;
		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const;
		VkImageView createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);


	};
}