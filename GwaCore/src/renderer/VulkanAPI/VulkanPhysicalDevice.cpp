#include "VulkanPhysicalDevice.h"
#include "QueueFamilyIndices.h"
#include "SwapchainDetails.h"
#include <stdexcept>
#include <vector>
#include <cassert>
namespace gwa{

	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface, std::shared_ptr<const std::vector<const char*>> deviceExtensions) : deviceExtensions(deviceExtensions)
	{
		std::vector<VkPhysicalDevice> deviceList = getPhysicalDeviceList(instance);
		// Check if device is suitable for this application
		int score = 0;
		int rating = 0;
		for (const auto& device : deviceList)
		{
			rating = ratePhysicalDeviceSuitable(device, surface);
			if (score < rating)
			{
				physicalDevice = device;
				score = rating;
			}
		}
		assert(physicalDevice);

		VkPhysicalDeviceProperties deviceProperties{};
		// NOT IN USE, for Dynamic UBO
		//minUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;
	}

	std::vector<VkPhysicalDevice> VulkanPhysicalDevice::getPhysicalDeviceList(VkInstance& instance) const
	{
			// Enumerate physical devices vkInstance can access
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("Can't find GPUs that support Vulkan Instance");
		}

		std::vector<VkPhysicalDevice> deviceList(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
		return deviceList;
	}


	int VulkanPhysicalDevice::ratePhysicalDeviceSuitable(const VkPhysicalDevice& curPhysicalDevice, VkSurfaceKHR& surface) const
	{
		int score = 0;
		// Get QueueFamily to check if valid for App
		QueueFamilyIndices indices = QueueFamilyIndices::getQueueFamilyIndices(curPhysicalDevice, surface);

		// Check if the right extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(curPhysicalDevice);

		bool swapChainValid = false;
	
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(curPhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(curPhysicalDevice, &deviceFeatures);
	
		SwapchainDetails swapchainDetails = SwapchainDetails::getSwapchainDetails(curPhysicalDevice, surface);
		swapChainValid = !swapchainDetails.presentationModes.empty() && !swapchainDetails.formats.empty();
		score = static_cast<int>(indices.isValid() && extensionsSupported && swapChainValid && deviceFeatures.geometryShader);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score *= 1000;
		}
		return score;
	}

	bool VulkanPhysicalDevice::checkDeviceExtensionSupport(const VkPhysicalDevice& curPhysicalDevice) const
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(curPhysicalDevice, nullptr, &extensionCount, nullptr);

		if (extensionCount == 0)
		{
			return false;
		}

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(curPhysicalDevice, nullptr, &extensionCount, extensions.data());
		
		for (const auto& deviceExtension : *deviceExtensions)
		{
			bool hasExtension = false;
			for (const auto& extension : extensions)
			{
				if (strcmp(deviceExtension, extension.extensionName) == 0)
				{
					hasExtension = true;
					break;
				}
			}

			if (!hasExtension)
			{
				return false;
			}
		}

		return true;
	}
}
