#include "VulkanDevice.h"
#include <cassert>
#include "VulkanValidation.h"
#include "GLFW/glfw3.h"
#include "SwapchainDetails.h"
#include "QueueFamilyIndices.h"
#include <set>

namespace gwa
{
	void VulkanDevice::init(const Window* const window, VkInstance instance, const std::vector<const char*>& deviceExtensions)
	{
		createSurface(window, instance);
		createPhysicalDevice(instance, deviceExtensions);
		createLogicalDevice(deviceExtensions);
	}

	void VulkanDevice::createSurface(const Window* const window, VkInstance instance)
	{
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->getWindowHandle());
		VkResult result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &vkSurface_);
		assert(result == VK_SUCCESS);
	}

	void VulkanDevice::createPhysicalDevice(VkInstance instance, const std::vector<const char*>& deviceExtensions)
	{
		std::vector<VkPhysicalDevice> deviceList = getPhysicalDeviceList(instance);
		// Check if device is suitable for this application
		int score = 0;
		int rating = 0;
		for (const VkPhysicalDevice& device : deviceList)
		{
			rating = ratePhysicalDeviceSuitable(device, deviceExtensions);
			if (score < rating)
			{
				vkPhysicalDevice_ = device;
				score = rating;
			}
		}
		assert(vkPhysicalDevice_);
	}
	std::vector<VkPhysicalDevice> VulkanDevice::getPhysicalDeviceList(VkInstance instance) const
	{
		// Enumerate physical devices vkInstance can access
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		assert(deviceCount > 0);
		std::vector<VkPhysicalDevice> deviceList(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
		return deviceList;
	}

	int VulkanDevice::ratePhysicalDeviceSuitable(VkPhysicalDevice currentPhysicalDevice, const std::vector<const char*>& deviceExtensions) const
	{
		int score = 0;
		// Get QueueFamily to check if valid for App
		QueueFamilyIndices indices = QueueFamilyIndices::getQueueFamilyIndices(currentPhysicalDevice, vkSurface_);

		// Check if the right extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(currentPhysicalDevice, deviceExtensions);

		bool swapChainValid = false;

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(currentPhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(currentPhysicalDevice, &deviceFeatures);

		SwapchainDetails swapchainDetails = SwapchainDetails::getSwapchainDetails(currentPhysicalDevice, vkSurface_);
		swapChainValid = !swapchainDetails.presentationModes.empty() && !swapchainDetails.formats.empty();
		score = static_cast<int>(indices.isValid() && extensionsSupported && swapChainValid && deviceFeatures.geometryShader);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score *= 1000;
		}
		return score;
	}

	bool VulkanDevice::checkDeviceExtensionSupport(const VkPhysicalDevice currentPhysicalDevice, const std::vector<const char*>& deviceExtensions) const
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(currentPhysicalDevice, nullptr, &extensionCount, nullptr);

		if (extensionCount == 0)
		{
			return false;
		}

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(currentPhysicalDevice, nullptr, &extensionCount, extensions.data());

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
	void VulkanDevice::createLogicalDevice(const std::vector<const char*>& deviceExtensions)
	{
		QueueFamilyIndices indices = QueueFamilyIndices::getQueueFamilyIndices(vkPhysicalDevice_, vkSurface_); //Which queues are supported?
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int>  queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };

		for (int queueFamilyIndex : queueFamilyIndices)
		{// Queue the logical device needs to create and info to do so (Only 1 for now, will add more later!)
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;			//The index of the family to create a queue from
			queueCreateInfo.queueCount = 1;									//Number of queues to create
			float priority = 1.f;
			queueCreateInfo.pQueuePriorities = &priority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t> (queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());					// Number and list of enabled logical device extensions
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		VkPhysicalDeviceFeatures phDeviceFeatures = {};
		deviceCreateInfo.pEnabledFeatures = &phDeviceFeatures;		//Set tessShader, etc here

		VkResult result = vkCreateDevice(vkPhysicalDevice_, &deviceCreateInfo, nullptr, &vkLogicalDevice_);
		assert(result == VK_SUCCESS);

		//Queues are created at the same time es the device
		// So we want handles to queues from given logical device of given Queue Familym of given Queue Index
		vkGetDeviceQueue(vkLogicalDevice_, indices.graphicsFamily, 0, &vkGraphicsQueue_);
		vkGetDeviceQueue(vkLogicalDevice_, indices.presentationFamily, 0, &vkPresentationQueue_);
	}
}
