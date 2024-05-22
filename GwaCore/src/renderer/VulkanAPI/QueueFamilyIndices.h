#include <vulkan/vulkan_core.h>
#include <vector>

namespace gwa {
	struct QueueFamilyIndices {
		int graphicsFamily = -1;
		int presentationFamily = -1;
		
		static QueueFamilyIndices getQueueFamilyIndices(const VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface) {
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyList.data());

			// Ceck if each queue family has at least one of the required queues
			int i = 0;
			for (const auto& queueFamily : queueFamilyList)
			{
				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					indices.graphicsFamily = i;
				}

				// Check if Queue Family supports presentation
				VkBool32 presentationSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);
				if (queueFamily.queueCount > 0 && presentationSupport)
				{
					indices.presentationFamily = i;
				}
				// Check if queue family indices are in a valid state, stop if so
				if (indices.isValid()) {
					break;
				}

				++i;
			}
			return indices;
		}

		bool isValid() const {
			return graphicsFamily >= 0 && presentationFamily >= 0;
		}
	};

}