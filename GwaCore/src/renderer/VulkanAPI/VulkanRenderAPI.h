#pragma once
#include <core/Window.h>
#include "renderer/RenderAPI.h"
#include "VulkanCommandBuffers.h"
#include "vkTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VulkanMeshBuffers.h"
#include "wrapper/VulkanInstance.h"
#include "wrapper/VulkanDevice.h"
#include "wrapper/VulkanSwapchain.h"
#include "wrapper/VulkanRenderPass.h"
#include "wrapper/VulkanDescriptorSetLayout.h"
#include "wrapper/VulkanPushConstant.h"
#include "wrapper/VulkanPipeline.h"
#include "wrapper/VulkanImage.h"
#include "wrapper/VulkanImageView.h"
#include "wrapper/VulkanSwapchainFramebuffers.h"
#include "wrapper/VulkanCommandPool.h"
#include "wrapper/VulkanDescriptorSet.h"
#include "wrapper/VulkanSemaphore.h"
#include "wrapper/VulkanFence.h"
#include "wrapper/VulkanUniformBuffers.h"

namespace gwa {
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		void init(const Window *  window, ResourceManager& resourceManager) override;
		void draw(const Window* window, const std::vector<TexturedMeshRenderObject>& meshes) override;
		void shutdown() override;

		void recordCommands(uint32_t imageIndex, const std::vector<TexturedMeshRenderObject>& renderObjects);
	private:
		const uint32_t maxFramesInFlight_ = 2;
		uint32_t currentFrame = 0;
		
		VulkanInstance m_instance;
		VulkanDevice m_device;
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
		VulkanDescriptorSet m_descriptorSet;
		VulkanSemaphore m_renderFinished;
		VulkanSemaphore m_imageAvailable;
		VulkanFence m_drawFences;
		VulkanMeshBuffers m_meshBuffers;

		const std::vector<const char*> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		
		//TODO temp fix, move Mesehs and camera to correct spot
		
	};
}

