#pragma once
#include <Core/Window.h>
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"
#include "renderer/RenderAPI.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanPushConstant.h"
#include "VulkanPipeline.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanSwapchainFramebuffers.h"
#include "VulkanCommandPool.h"

#include <glm/glm.hpp>
namespace gwa {
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		void init(Window * window) override;
		void shutdown() override;

	private:
		
		std::unique_ptr<VulkanSurface> m_surface;
		std::unique_ptr<VulkanInstance> m_instance;
		std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
		std::unique_ptr<VulkanLogicalDevice> m_logicalDevice;
		std::unique_ptr<VulkanSwapchain> m_swapchain;
		std::unique_ptr<VulkanRenderPass> m_renderPass;
		std::unique_ptr<VulkanDescriptorSetLayout> m_descriptorSetLayout;
		std::unique_ptr<VulkanPushConstant> m_pushConstant;
		std::unique_ptr<VulkanPipeline> m_graphicsPipeline;
		std::unique_ptr<VulkanImage> m_depthBufferImage;
		std::unique_ptr<VulkanImageView> m_depthBufferImageView;
		std::unique_ptr<VulkanSwapchainFramebuffers> m_swapchainFramebuffers; 
		std::unique_ptr<VulkanCommandPool> m_graphicsCommandPool;

		std::shared_ptr<const std::vector<const char*>> deviceExtensions = 
			std::make_shared<const std::vector<const char*>>(std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME});
		
		//TODO temp fix, move Mesehs and camera to correct spot
		struct Model
		{
			glm::mat4 model;
		};
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 col;
		};
	};
}

