#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
namespace gwa
{

	class VulkanInstance
	{
	public:
		VulkanInstance(const std::string& appName, uint32_t appVersion, const std::string& engineName,
			uint32_t engineVersion, uint32_t apiVersion, const std::vector<const char*>* validationLayers);
		~VulkanInstance();

		void cleanup();

		bool checkValidationLayerSupport() const;

		VkInstance vkInstance;
	private:
		std::vector<const char*> getRequiredExtensions() const;
		bool checkInstanceExtensionSupport(const std::vector<const char*>* const checkExtensions) const;

		void createDebugMessenger();

		const std::vector<const char*>* validationLayers;
		const bool enableValidationLayers;
		VkDebugUtilsMessengerEXT debugMessenger;

	};
}