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
#include "wrapper/VulkanBindlessDescriptor.h"
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
#include "TextureImage.h"
#include "wrapper/VulkanImageSampler.h"
#include "wrapper/VulkanImguiIntegration.h"

namespace gwa::renderer {
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		void init(const Window *  window, gwa::ntity::Registry& registry) override;
		void draw(const Window* window, gwa::ntity::Registry& registry) override;
		void shutdown() override;

		void recordCommands(uint32_t imageIndex, gwa::ntity::Registry& registry);
	private:
		void recreateSwapchain(WindowSize framebufferSize);
		const uint32_t maxFramesInFlight_ = 2;
		uint32_t currentFrame = 0;
		
		VulkanInstance m_instance;
		VulkanDevice m_device;
		VulkanSwapchain m_swapchain;
		VulkanRenderPass m_renderPass;
		VulkanDescriptorSetLayout m_descriptorSetLayout;
		VulkanPushConstant m_pushConstant;
		VulkanPipeline m_graphicsPipeline;
		VulkanPipeline m_graphicsPipelineTransparent;
		VulkanImage m_depthBufferImage;
		VulkanImageView m_depthBufferImageView;
		VulkanSwapchainFramebuffers m_swapchainFramebuffers; 
		VulkanCommandPool m_graphicsCommandPool;
		std::vector<VulkanCommandBuffer> m_graphicsCommandBuffers;
		VulkanUniformBuffers m_mvpUniformBuffers;
		VulkanDescriptorSet m_descriptorSet;
		VulkanSemaphore m_renderFinished;
		VulkanSemaphore m_imageAvailable;
		VulkanFence m_drawFences;
		VulkanMeshBuffers m_meshBuffers;
		std::vector<TextureImage> m_textures;
		VulkanImageView m_textureViews;
		VulkanImageSampler m_textureSampler;
		VulkanImguiIntegration m_imgui;

		struct PushConstant
		{
			glm::mat4 model;
			uint32_t textureIndex;
		} pushConstantObject;

		const std::vector<const char*> deviceExtensions{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

	};
}

