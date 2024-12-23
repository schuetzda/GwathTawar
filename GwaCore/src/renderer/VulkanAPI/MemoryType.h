#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <cassert>
namespace gwa
{
	class MemoryType
	{
	public:
		static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

			for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
			{
				if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					// This memoryType is Valid so return index
					return i;
				}
			}
			throw std::runtime_error("failed to find suitable memory type!");
			return 0;
		}
	};
}