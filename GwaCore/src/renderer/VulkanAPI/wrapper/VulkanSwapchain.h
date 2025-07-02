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
		VulkanSwapchain(const VulkanDevice* device, int framebufferWidth, int framebufferHeight);
		~VulkanSwapchain() = default;

		void cleanup(VkDevice logicalDevice);
		void recreateSwapchain(const VulkanDevice* device, int framebufferWidth, int framebufferHeight);

		VkSwapchainKHR getSwapchain() const
		{
			return vkSwapchain_;
		}
		const VkSwapchainKHR* getSwapchainPtr()
		{
			return  &vkSwapchain_;
		}
		VkFormat getImageFormat() const
		{
			return vkSwapchainImageFormat_;
		}
		VkExtent2D getSwapchainExtent() const
		{
			return vkSwapchainExtent_;
		}
		uint32_t getSwapchainImagesSize() const
		{
			return static_cast<uint32_t>(swapchainImages.size());
		}

		const std::vector<VkImage>& getSwapchainImages() const
		{
			return swapchainImages;
		}

	private:
		void createSwapchain(const VulkanDevice* device, int framebufferWidth, int framebufferHeight, bool recreate);
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) const;
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, int framebufferWidth, int framebufferHeight) const;
		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const;
		VkImageView createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);


		VkSwapchainKHR vkSwapchain_{};
		VkFormat vkSwapchainImageFormat_{};
		VkExtent2D vkSwapchainExtent_{};
		std::vector<VkImage> swapchainImages;
	};
}