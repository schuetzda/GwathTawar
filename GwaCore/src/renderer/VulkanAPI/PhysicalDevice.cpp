#include "PhysicalDevice.h"
#include "QueueFamilyIndices.h"
#include "SwapchainDetails.h"
#include <stdexcept>
#include <vector>
#include <cassert>
namespace gwa{

	PhysicalDevice::PhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface)
	{
		std::vector<VkPhysicalDevice> deviceList = getPhysicalDeviceList(instance);
		// Check if device is suitable for this application
		for (const auto& device : deviceList)
		{
			if (checkPhysicalDeviceSuitable(device, surface))
			{
				physicalDevice = std::make_unique<VkPhysicalDevice>(device);
				break;
			}
		}
		assert(physicalDevice);

		VkPhysicalDeviceProperties deviceProperties;
		// NOT IN USE, for Dynamic UBO
		//minUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;
	}

	std::vector<VkPhysicalDevice> PhysicalDevice::getPhysicalDeviceList(VkInstance& instance) const 
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


	bool PhysicalDevice::checkPhysicalDeviceSuitable(const VkPhysicalDevice& curPhysicalDevice, VkSurfaceKHR& surface) const
	{
		// Get QueueFamily to check if valid for App
		QueueFamilyIndices indices = QueueFamilyIndices::getQueueFamilyIndices(curPhysicalDevice, surface);

		// Check if the right extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(curPhysicalDevice);

		bool swapChainValid = false;
		
		SwapchainDetails swapchainDetails = SwapchainDetails::getSwapchainDetails(curPhysicalDevice, surface);
		swapChainValid = !swapchainDetails.presentationModes.empty() && !swapchainDetails.formats.empty();

		return indices.isValid() && extensionsSupported && swapChainValid;
	}

	bool PhysicalDevice::checkDeviceExtensionSupport(const VkPhysicalDevice& curPhysicalDevice) const
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(curPhysicalDevice, nullptr, &extensionCount, nullptr);

		if (extensionCount == 0)
		{
			return false;
		}

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(curPhysicalDevice, nullptr, &extensionCount, extensions.data());
		
		for (const auto& deviceExtension : deviceExtensions)
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
