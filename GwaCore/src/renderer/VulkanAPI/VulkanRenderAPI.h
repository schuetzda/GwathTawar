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

		/**
		 * @brief Initializes all resources of the Vulkan API
		 * @param window Window instance that will be rendered too
		 * @param registry Entity Component System to store and load relevant resources
		 * @param description Render graph description that specifies the architecture of the render engine that will be implemented in Vulkan.
		*/
		void init(const Window *  window, gwa::ntity::Registry& registry, const RenderGraphDescription& description) override;

		/**
		 * @brief Renders a single frame using the Vulkan API.
		 * @param window Pointer to the window to render to. Used to get framebuffer size and context.
		 * @param registry The ECS registry containing all renderable entities for the frame.
		 * @note If the framebuffer size is zero (e.g., window is minimized), the function 
		 *       skips rendering and only updates the ImGui platform windows if enabled.
		 * @note This method assumes that Vulkan resources such as command buffers,
		 *       semaphores, fences, and swapchain have been properly initialized.
		*/
		void draw(const Window* window, gwa::ntity::Registry& registry) override;

		/**
		 * @brief Releases all Vulkan specific objects and their memory
		 */
		void shutdown() override;

	private:
		/**
		 * @brief Records Vulkan command buffers for rendering the current frame.
		 *
		 * @param imageIndex Index of the swapchain image to render into.
		 * @param registry Reference to the ECS registry containing renderable entities and their components.
		 */
		void recordCommands(uint32_t imageIndex, gwa::ntity::Registry& registry);

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
			std::vector<uint32_t> meshesToRender;
			bool useDepthBuffer{ false };
			bool renderFullscreenPass{ false };
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

		/**
		 * @brief Recreates the Vulkan swapchain and all associated framebuffer resources.
		 *
		 * @param framebufferSize The new dimensions of the window's framebuffer.
		 */
		void recreateSwapchain(WindowSize framebufferSize);

		/**
		 * @brief Loads and prepares textured meshes for rendering from a render graph node.
		 *
		 * @param curDataNode The data node used to store GPU-side mesh buffer memory and textures.
		 * @param curRenderNode The render node that holds mesh buffer references and renderable entity IDs.
		 * @param curRenderGraphNode The current node in the render graph describing which textured meshes to load.
		 * @param registry The ECS registry for managing resources and entities.
		 * @param resourceAttachments A map of resource handles (mesh/texture) used to resolve mesh and texture data.
		 *
		 * @note A default texture is added at index 0 and used as a fallback when a material texture is missing.
		 */
		void loadTexturedMeshes(DataNode& curDataNode, RenderNode& curRenderNode, const RenderGraphNode& curRenderGraphNode, gwa::ntity::Registry& registry, const std::map<size_t, ResourceAttachment>& resourceAttachments) const;

		/**
		*@brief Creates a framebuffer attachment(color or depth) and its corresponding image views.
		*
		* @param curDataNode Reference to the current DataNode storing framebuffer images and views.
		* @param attachmentHandle Unique handle identifying this framebuffer attachment(used as a key).
		* @param format The Vulkan image format to use(e.g., VK_FORMAT_D32_SFLOAT).
		* @param attachmentSize The size of the attachment(usually swapchain extent).
		* @param aspectFlag Aspect of the image the view will access(e.g., VK_IMAGE_ASPECT_COLOR_BIT or DEPTH_BIT).
		* @param usageBits Usage flags for the image(e.g., VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT).
		*
		* @note The created image views are inserted into `curDataNode.frameBufferImageViews`
		* and are also referenced in `framebufferImageViewsReference` using the given handle.
		*/
		void createFramebufferAttachment(DataNode& curDataNode, size_t attachmentHandle, VkFormat format, VkExtent2D attachmentSize, VkImageAspectFlagBits aspectFlag, VkImageUsageFlags usageBits);

		uint32_t currentFrame = 0;


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

