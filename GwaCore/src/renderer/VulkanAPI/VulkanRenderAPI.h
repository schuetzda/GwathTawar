#pragma once
#include <Core/Window.h>
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "PhysicalDevice.h"
#include "renderer/RenderAPI.h"

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
		std::unique_ptr<PhysicalDevice> m_physicalDevice;
	};
}

