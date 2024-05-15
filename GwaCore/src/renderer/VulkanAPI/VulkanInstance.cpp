#include "VulkanInstance.h"
#include <cassert>
#include <vector>
#include "VulkanValidation.h"
#include <GLFW/glfw3.h>

namespace gwa
{
	VulkanInstance::VulkanInstance(const std::string& appName, uint32_t appVersion, const std::string& engineName,
		uint32_t engineVersion, uint32_t apiVersion, const std::vector<const char*> * validationLayers) 
		: validationLayers(validationLayers), enableValidationLayers(!validationLayers->empty())
	{
		if (enableValidationLayers) {
			assert(checkValidationLayerSupport());
		}
			
		
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName.c_str();
		// Whatever the version of your application is
		appInfo.applicationVersion = appVersion;
		appInfo.pEngineName = engineName.c_str(); // Custom Name of the used Engine
		appInfo.engineVersion = engineVersion;
		appInfo.apiVersion = apiVersion;

		VkInstanceCreateInfo createInfo = {  };
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers->size());
			createInfo.ppEnabledLayerNames = validationLayers->data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// List of all extensions used
		std::vector<const char*> instanceExtensions = getRequiredExtensions();

		// Check Extensions of the supported instance
		assert(checkInstanceExtensionSupport(&instanceExtensions));

		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();


		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers->size());
			createInfo.ppEnabledLayerNames = validationLayers->data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);		// Callback for memory management 2nd argument

		assert(result == VK_SUCCESS);

		if (enableValidationLayers)
		{
			createDebugMessenger();
		}

	}

	VulkanInstance::~VulkanInstance()
	{
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
		}

		vkDestroyInstance(vkInstance, nullptr);
		
	}

	bool VulkanInstance::checkValidationLayerSupport() const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const std::string layerName : (*validationLayers))
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName.c_str(), layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanInstance::getRequiredExtensions() const
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
	}

	bool VulkanInstance::checkInstanceExtensionSupport(const std::vector<const char*> * const checkExtensions) const
	{
		// First get the number of extension in order to populate our vector
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		for (const auto& checkExtension : *checkExtensions)
		{
			bool hasExtensions = false;
			for (const auto& extension : extensions)
			{
				if (strcmp(checkExtension, extension.extensionName))
				{
					hasExtensions = true;
					break;
				}
			}

			if (!hasExtensions) {
				return false;
			}
		}

		return true;
	}

	void VulkanInstance::createDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		assert(!CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS);
	}
}
