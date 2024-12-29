#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <core/Window.h>
namespace gwa
{
	class VulkanDevice
	{
	public:
		VulkanDevice() = default;

		void init(const Window* const window, VkInstance instance, const std::vector<const char*>& deviceExtensions);

		VkSurfaceKHR getSurface() const
		{
			return vkSurface_;
		}

		VkPhysicalDevice getPhysicalDevice() const
		{
			return vkPhysicalDevice_;
		}

		VkDevice getLogicalDevice() const
		{
			return vkLogicalDevice_;
		}

		VkQueue getGraphicsQueue() const
		{
			return vkGraphicsQueue_;
		}

		VkQueue getPresentationQueue() const
		{
			return vkPresentationQueue_;
		}
	private:
		void createSurface(const Window* const window, VkInstance instance);


		void createPhysicalDevice(VkInstance instance, const std::vector<const char*>& deviceExtensions);
		std::vector<VkPhysicalDevice> getPhysicalDeviceList(VkInstance instance) const;
		int ratePhysicalDeviceSuitable(VkPhysicalDevice currentPhysicalDevice, const std::vector<const char*>& deviceExtensions) const;
		bool checkDeviceExtensionSupport(VkPhysicalDevice currentPhysicalDevice, const std::vector<const char*>& deviceExtensions) const;

		void createLogicalDevice(const std::vector<const char*>& deviceExtensions);
		
		VkSurfaceKHR vkSurface_;
		
		VkPhysicalDevice vkPhysicalDevice_;

		VkDevice vkLogicalDevice_;
		VkQueue vkGraphicsQueue_;
		VkQueue vkPresentationQueue_;

	};
}
