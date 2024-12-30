#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
namespace gwa
{

	class VulkanInstance
	{
	public:
		VulkanInstance() = default;
		VulkanInstance(const std::string& appName, const std::string& engineName, uint32_t appVersion, uint32_t engineVersion, uint32_t apiVersion, 
			const std::vector<const char*>* validationLayers);
		~VulkanInstance() = default;
		
		void cleanup();
		bool checkValidationLayerSupport(const std::vector<const char*>* validationLayers) const;
		
		VkInstance getVkInstance()
		{
			return vkInstance_;
		}
	private:
		std::vector<const char*> getRequiredExtensions() const;
		bool checkInstanceExtensionSupport(const std::vector<const char*>* const checkExtensions) const;

		void createDebugMessenger();

		bool m_enableValidationLayers = false;
		VkDebugUtilsMessengerEXT m_debugMessenger{};

		VkInstance vkInstance_{};
	};
}