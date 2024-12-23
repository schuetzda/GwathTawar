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
#include "VulkanDescriptorSet.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "MeshBuffer.h"
#include "vkTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gwa {
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		void init(const Window *  window) override;
		void draw(const Window *  window) override;
		void shutdown() override;

		void recordCommands(uint32_t imageIndex);
		void updateModel(int modelId, const glm::mat4& newModel) override;

	private:
		void recordCommands();

		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		
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
		std::unique_ptr<VulkanCommandBuffers> m_graphicsCommandBuffer;
		std::unique_ptr<VulkanUniformBuffers> m_mvpUniformBuffers;
		std::unique_ptr<VulkanDescriptorSet> m_descriptorSet;
		std::unique_ptr<VulkanSemaphore> m_renderFinished;
		std::unique_ptr<VulkanSemaphore> m_imageAvailable;
		std::unique_ptr<VulkanFence> m_drawFences;

		std::vector<MeshBuffer> meshes;

		std::shared_ptr<const std::vector<const char*>> deviceExtensions = 
			std::make_shared<const std::vector<const char*>>(std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME});
		
		//TODO temp fix, move Mesehs and camera to correct spot
		struct UboViewProj {
			glm::mat4 projection;
			glm::mat4 view;
		};		
		UboViewProj uboViewProj;
	};
}

