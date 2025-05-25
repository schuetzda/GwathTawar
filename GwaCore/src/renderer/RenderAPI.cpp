#include "RenderAPI.h"
#include "VulkanAPI/VulkanRenderAPI.h"

namespace gwa::renderer {
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::Vulkan;

	
	std::unique_ptr<RenderAPI> RenderAPI::Create()
	{
		return std::make_unique<VulkanRenderAPI>();
	}

}