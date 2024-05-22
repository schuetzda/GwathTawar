#pragma once
#include <Core/Window.h>
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"
#include "renderer/RenderAPI.h"
#include "VulkanLogicalDevice.h"

namespace gwa {
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		virtual void init(Window * window) override;
		virtual void shutdown() override;

	private:
		
		std::unique_ptr<VulkanSurface> m_surface;
		std::unique_ptr<VulkanInstance> m_instance;
		std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
		std::unique_ptr<VulkanLogicalDevice> m_logicalDevice;
		
		std::shared_ptr<const std::vector<const char*>> deviceExtensions = 
			std::make_shared<const std::vector<const char*>>(std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME});

	};
}

