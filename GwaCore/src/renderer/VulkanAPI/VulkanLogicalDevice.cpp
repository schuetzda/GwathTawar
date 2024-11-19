#include "VulkanLogicalDevice.h"
#include "QueueFamilyIndices.h"
#include <set>
#include <stdexcept>
namespace gwa {
	VulkanLogicalDevice::VulkanLogicalDevice(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, std::shared_ptr<const std::vector<const char*>> deviceExtensions)
		: deviceExtensions(deviceExtensions)
	{
		QueueFamilyIndices indices = QueueFamilyIndices::getQueueFamilyIndices(physicalDevice, surface); //Which queues are supported?
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
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions->size());					// Number and list of enabled logical device extensions
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions->data();

		VkPhysicalDeviceFeatures phDeviceFeatures = {};
		deviceCreateInfo.pEnabledFeatures = &phDeviceFeatures;		//Set tessShader, etc here

		VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create a logical device!");
		}

		//Queues are created at the same time es the device
		// So we want handles to queues from given logical device of given Queue Familym of given Queue Index
		vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.presentationFamily, 0, &presentationQueue);

	}
	void VulkanLogicalDevice::cleanup()
	{
		vkDestroyDevice(logicalDevice, nullptr);
	}
}
