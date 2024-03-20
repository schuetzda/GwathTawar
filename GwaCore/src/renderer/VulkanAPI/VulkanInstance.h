#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
namespace gwa
{

	class VulkanInstance
	{
	public:
		VulkanInstance(const std::string& appName, uint32_t appVersion,const std::string& engineName, 
			uint32_t engineVersion, uint32_t apiVersion, const std::vector<const char*> * validationLayers);
		~VulkanInstance();
		bool checkValidationLayerSupport() const;


	private:
		std::vector<const char*> getRequiredExtensions() const;
		bool checkInstanceExtensionSupport(std::vector<const char*> * const checkExtensions) const;

		const std::vector<const char*> * validationLayers;
		const bool enableValidationLayers;
		VkInstance instance;
	};
}