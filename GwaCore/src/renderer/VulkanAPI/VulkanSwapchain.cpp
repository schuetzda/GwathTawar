#include "VulkanSwapchain.h"
#include "SwapchainDetails.h"
#include "QueueFamilyIndices.h"
#include <iostream>
#include <cassert>
#include <array>
namespace gwa {

	VulkanSwapchain::VulkanSwapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR vkSurface, int framebufferWidth, int framebufferHeight)
	{
		createSwapchain(logicalDevice, physicalDevice, vkSurface, framebufferWidth, framebufferHeight);
	}

	void VulkanSwapchain::createSwapchain(VkDevice vkLogicalDevice, VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurface, int framebufferWidth, int framebufferHeight)
	{
		SwapchainDetails swapchainDetails = SwapchainDetails::getSwapchainDetails(vkPhysicalDevice, vkSurface);

		VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapchainDetails.formats);
		VkPresentModeKHR presentMode = chooseBestPresentationMode(swapchainDetails.presentationModes);
		VkExtent2D extent = chooseSwapExtent(swapchainDetails.surfaceCababilities, framebufferWidth, framebufferHeight);

		// How many images are in the swap chain? Get 1 more than the minimum to allow triple buffering
		uint32_t imageCount = swapchainDetails.surfaceCababilities.minImageCount + 1;

		if (swapchainDetails.surfaceCababilities.maxImageCount > 0 && swapchainDetails.surfaceCababilities.maxImageCount < imageCount)
		{
			imageCount = swapchainDetails.surfaceCababilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = vkSurface;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.imageExtent = extent;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageArrayLayers = 1;		// Number of layers for each image in chain
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	//What attachment images will be used as
		swapChainCreateInfo.preTransform = swapchainDetails.surfaceCababilities.currentTransform;	// Transform to perform on swap chain images
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		// How to handle blending images with external graphic (e.g. other windows)
		swapChainCreateInfo.clipped = VK_TRUE;	//Whether to clip parts of image not in view (e.g. behind another window, off screen)

		QueueFamilyIndices indices = QueueFamilyIndices::getQueueFamilyIndices(vkPhysicalDevice, vkSurface);

		if (indices.graphicsFamily != indices.presentationFamily)
		{
			std::array<uint32_t,2> queueFamilyIndices {
				(uint32_t)indices.graphicsFamily,
				(uint32_t)indices.presentationFamily
			};

			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}
		else
		{
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainCreateInfo.queueFamilyIndexCount = 0;
			swapChainCreateInfo.pQueueFamilyIndices = nullptr;
		}

		// If old swap chain is destroyed and this one replaces it, then link old one to quickly hand over responsibilites
		//swapChainCreateInfo.oldSwapchain = swapchain;

		VkResult result = vkCreateSwapchainKHR(vkLogicalDevice, &swapChainCreateInfo, nullptr, &vkSwapchain);
		assert(result == VK_SUCCESS);
		//Store for later references
		vkSwapchainImageFormat = surfaceFormat.format;
		vkSwapchainExtent = extent;

		// Get Swapchain images
		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(vkLogicalDevice, vkSwapchain, &swapchainImageCount, nullptr);
		std::vector<VkImage> images(swapchainImageCount);
		vkGetSwapchainImagesKHR(vkLogicalDevice, vkSwapchain, &swapchainImageCount, images.data());

		for (VkImage image : images)
		{
			VulkanSwapchainImage swapchainImage = {};
			swapchainImage.image = image;
			swapchainImage.imageView = createImageView(vkLogicalDevice, image, vkSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

			swapchainImages.push_back(swapchainImage);
		}
	}
	VkPresentModeKHR VulkanSwapchain::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) const
	{
		for (const auto& presentationMode : presentationModes)
		{
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return presentationMode;
			}
		}

		// This has to be present by default standard
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, int framebufferWidth, int framebufferHeight) const
	{
		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return surfaceCapabilities.currentExtent;
		}
		else
		{
			VkExtent2D newExtent = {};
			newExtent.width = static_cast<uint32_t>(framebufferWidth);
			newExtent.height = static_cast<uint32_t>(framebufferHeight);

			//Surace also defines max and minn, so make sure within boundaries by clamping value
			newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
			newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
			return newExtent;
		}
	}

	VkSurfaceFormatKHR VulkanSwapchain::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const
	{
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& format : formats)
		{
			if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		std::cerr << "Can't find suitable Surface Format";
		return formats[0];
	}

	VkImageView VulkanSwapchain::createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = image;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = format;
		// Allows remappung of rgba compononents to other rgba values
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//Subresources allow the view to view only a part of an image
		viewCreateInfo.subresourceRange.aspectMask = aspectFlags;		//Wich aspect of image to view (e.g. COLOR_BIT for view color)
		viewCreateInfo.subresourceRange.baseMipLevel = 0;				// Start mipmap level to view from
		viewCreateInfo.subresourceRange.levelCount = 1;					// Number of mipmap levels to view
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;				// Start array level to view from
		viewCreateInfo.subresourceRange.layerCount = 1;					// number of array levels to view

		//Create image view and return it
		VkImageView imageView;
		VkResult result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &imageView);
		assert(result == VK_SUCCESS);

		return imageView;
	}

	VulkanSwapchain::~VulkanSwapchain() = default;

	void VulkanSwapchain::cleanup(VkDevice logicalDevice) 
	{
		vkDeviceWaitIdle(logicalDevice);
		for (auto image : swapchainImages)
		{
			vkDestroyImageView(logicalDevice, image.imageView, nullptr);
		}
		vkDestroySwapchainKHR(logicalDevice, vkSwapchain, nullptr);
	}

	void VulkanSwapchain::recreateSwapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int framebufferWidth, int framebufferHeight) 
	{
		cleanup(logicalDevice);
		swapchainImages.clear();
		
		createSwapchain(logicalDevice, physicalDevice, surface, framebufferWidth, framebufferHeight);
	}


}
