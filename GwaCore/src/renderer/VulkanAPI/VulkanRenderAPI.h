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
#include "wrapper/VulkanImageViewCollection.h"
#include "wrapper/VulkanFramebuffers.h"
#include "wrapper/VulkanCommandPool.h"
#include "wrapper/VulkanDescriptorSet.h"
#include "wrapper/VulkanSemaphore.h"
#include "wrapper/VulkanFence.h"
#include <vector>
#include "wrapper/VulkanUniformBuffers.h"
#include "TextureImage.h"
#include "wrapper/VulkanImageSampler.h"
#include "wrapper/VulkanImguiIntegration.h"
#include "wrapper/VulkanImageView.h"

namespace gwa::renderer {
	constexpr uint32_t maxFramesInFlight_ = 2;
	class VulkanRenderAPI: public RenderAPI 
	{
	public:
		VulkanRenderAPI() = default;

		void init(const Window *  window, gwa::ntity::Registry& registry, const RenderGraphDescription& description) override;
		void draw(const Window* window, gwa::ntity::Registry& registry) override;
		void shutdown() override;

		void recordCommands(uint32_t imageIndex, gwa::ntity::Registry& registry);
	private:
		void recreateSwapchain(WindowSize framebufferSize);
		uint32_t currentFrame = 0;

		struct RenderNode
		{
			VulkanRenderPass renderPass{};
			VulkanDescriptorSetLayout descriptorSetLayout{};
			VulkanPushConstant pushConstant{};
			VulkanPipeline pipeline{};
			VulkanDescriptorSet descriptorSet{};
			VulkanMeshBuffers meshBuffers{};
			VulkanFramebuffers framebuffers;
			std::vector<VulkanUniformBuffers> uniformBuffers;
			bool useDepthBuffer{ false };
		};

		struct DataNode
		{
			VulkanMeshBufferMemory meshBuffersMemory;
			std::vector<TextureImage> textures{};
			VulkanImageViewCollection textureViews{};
			std::vector<VulkanImage> framebufferImages;
			std::vector<VulkanImageViewCollection> frameBufferImageViews;
			std::vector<size_t> renderAttachmentHandles;
		};

		std::vector<std::unordered_map<size_t, VkImageView>> framebufferImageViewsReference;
		std::map<size_t, RenderAttachment> attachmentInfos;

		
		std::vector<RenderNode> renderNodes{};
		std::vector<DataNode> dataNodes{};
		
		VulkanInstance m_instance{};
		VulkanDevice m_device{};
		VulkanSwapchain m_swapchain{};

		VulkanCommandPool m_graphicsCommandPool{};
		std::vector<VulkanCommandBuffer> m_graphicsCommandBuffers{};
		VulkanSemaphore m_renderFinished{};
		VulkanSemaphore m_imageAvailable{};
		VulkanFence m_drawFences{};
		VulkanImageSampler m_textureSampler{};
		VulkanImageSampler m_framebufferSampler{};
		VulkanImguiIntegration m_imgui{};

		VkFormat depthFormat = VkFormat::VK_FORMAT_UNDEFINED;

		struct PushConstant
		{
			glm::mat4 model;
			uint32_t textureIndex;
		} pushConstantObject;

		struct RenderObjects
		{
			VulkanRenderPass renderpass;
			VkPipeline pipeline;
		};

		const std::vector<const char*> deviceExtensions{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

	};
}

