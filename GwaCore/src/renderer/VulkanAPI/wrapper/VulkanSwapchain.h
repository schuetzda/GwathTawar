/* Swapchain
List of image buffers that the GPU draws into.
Those are presented to the display hardware 
*/
#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanDevice.h"

namespace gwa {
	struct VulkanSwapchainImage {
		VkImage image;
		VkImageView imageView;
	};

	class VulkanSwapchain {

	public:
		VulkanSwapchain() = default;
		~VulkanSwapchain() = default;

		void init(VulkanDevice* const device, int framebufferWidth, int framebufferHeight);
		void cleanup();
		void recreateSwapchain(int framebufferWidth, int framebufferHeight);

		VkSwapchainKHR getSwapchain() const
		{
			return vkSwapchain_;
		}
		VkFormat getImageFormat() const
		{
			return vkSwapchainImageFormat_;
		}
		VkExtent2D getSwapchainExtent() const
		{
			return vkSwapchainExtent_;
		}
		const std::vector<VulkanSwapchainImage>& getSwapchainImages() const
		{
			return swapchainImages_;
		}

	private:
		void createSwapchain(int framebufferWidth, int framebufferHeight);
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) const;
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, int framebufferWidth, int framebufferHeight) const;
		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const;
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	
		VkSwapchainKHR vkSwapchain_;
		VkFormat vkSwapchainImageFormat_;
		VkExtent2D vkSwapchainExtent_;
		std::vector<VulkanSwapchainImage> swapchainImages_;

		VulkanDevice* const device_;
	};
}