#pragma once
#include <core/Window.h>
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
#include "VulkanCommandBuffers.h"
#include "VulkanUniformBuffers.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"

#include <glm/glm.hpp>
namespace gwa {
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		void init(Window * window) override;
		void draw() override;
		void shutdown() override;

	private:
		
		VulkanSurface m_surface;
		VulkanInstance m_instance;
		VulkanPhysicalDevice m_physicalDevice;
		VulkanLogicalDevice m_logicalDevice;
		VulkanSwapchain m_swapchain;
		VulkanRenderPass m_renderPass;
		VulkanDescriptorSetLayout m_descriptorSetLayout;
		VulkanPushConstant m_pushConstant;
		VulkanPipeline m_graphicsPipeline;
		VulkanImage m_depthBufferImage;
		VulkanImageView m_depthBufferImageView;
		VulkanSwapchainFramebuffers m_swapchainFramebuffers; 
		VulkanCommandPool m_graphicsCommandPool;
		VulkanCommandBuffers m_graphicsCommandBuffer;
		VulkanUniformBuffers m_mvpUniformBuffers;
		VulkanDescriptorPool m_descriptorPool;
		VulkanDescriptorSet m_descriptorSet;

		std::shared_ptr<const std::vector<const char*>> deviceExtensions = 
			std::make_shared<const std::vector<const char*>>(std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME});
		
		//TODO temp fix, move Mesehs and camera to correct spot
		struct Model
		{
			glm::mat4 model;
		};
		struct UboViewProj {
			glm::mat4 projection;
			glm::mat4 view;
		};
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 col;
		};
	};
}

