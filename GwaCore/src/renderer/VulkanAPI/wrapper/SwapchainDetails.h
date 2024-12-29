#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace gwa {
	struct SwapchainDetails {
		VkSurfaceCapabilitiesKHR surfaceCababilities = VkSurfaceCapabilitiesKHR();		//Surface properties, e.g. image size/extent
		std::vector<VkSurfaceFormatKHR> formats;			//Surface image formats, e.g. RGBA
		std::vector<VkPresentModeKHR> presentationModes;	//How images should be presented to screen

		static SwapchainDetails getSwapchainDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
			SwapchainDetails swapChainDetails;

			// CAPABILITIES
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainDetails.surfaceCababilities);

			// FORMATS
			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

			if (formatCount > 0)
			{
				swapChainDetails.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapChainDetails.formats.data());
			}

			// PRESENTATION MODES
			uint32_t presentationCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationCount, nullptr);

			if (presentationCount > 0)
			{
				swapChainDetails.presentationModes.resize(presentationCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationCount, swapChainDetails.presentationModes.data());
			}

			return swapChainDetails;
		}
	};
};
