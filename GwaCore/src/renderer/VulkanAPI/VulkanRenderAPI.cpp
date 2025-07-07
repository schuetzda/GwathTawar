#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <io/TextureReader.h>

#include "VulkanRenderAPI.h"
#include <array>
#include <stdexcept>
#include <span>
#include <glm/gtc/matrix_transform.hpp>
#include "ecs/Registry.h"
#include "wrapper/VulkanUtility.h"
#include "ecs/components/RenderObjects.h"
#include "wrapper/QueueFamilyIndices.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>
#include "renderer/rendergraph/RenderGraph.h"
#include <renderer/rendergraph/PipelineBuilder.h>
#include "renderer/rendergraph/DescriptorSetConfigurator.h"


namespace gwa::renderer {
	void VulkanRenderAPI::init(const Window* window, gwa::ntity::Registry& registry, const RenderGraphDescription& description)
	{

		// Create Vulkan specific objects and resources
#ifdef GWA_DEBUG
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#else
		const std::vector<const char*> validationLayers;
#endif
		m_instance = VulkanInstance(window->getAppTitle(), std::string("Gwa Engine"), VK_MAKE_API_VERSION(1, 3, 0, 0),
			VK_MAKE_API_VERSION(0, 1, 0, 0), VK_API_VERSION_1_3, &validationLayers);
		const WindowSize framebufferSize = window->getFramebufferSize();
		m_device = VulkanDevice(window, m_instance.getVkInstance(), deviceExtensions);
		m_swapchain = VulkanSwapchain(&m_device, framebufferSize);
		m_graphicsCommandPool = VulkanCommandPool(&m_device);
		m_graphicsCommandBuffers = vulkanutil::initCommandBuffers(m_device.getLogicalDevice(), m_graphicsCommandPool.getCommandPool(), maxFramesInFlight_);
		m_textureSampler = VulkanImageSampler(m_device.getLogicalDevice(), m_device.getPhysicalDevice());
		m_framebufferSampler = VulkanImageSampler(m_device.getLogicalDevice(), m_device.getPhysicalDevice());
		attachmentInfos = description.renderAttachments;

		// Set up render pass specific classes and resources. RenderNodes will be later used in the draw call of the Vulkan Engine. Data node includes Vulkan related resources which maybe needed in f.e. a window resize.
		const size_t nodeCount = description.graphNodes.size();
		renderNodes.resize(nodeCount);
		dataNodes.resize(nodeCount);
		//Keep references to the image view of the different render passes, so that a later render pass can sample from a previous rendered image.	
		framebufferImageViewsReference.resize(m_swapchain.getSwapchainImagesSize());
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
		{
			const RenderGraphNode curRenderGraphNode = description.graphNodes[nodeIndex];
			RenderNode& curRenderNode = renderNodes[nodeIndex];
			DataNode& curDataNode = dataNodes[nodeIndex];

			if (curRenderGraphNode.pipelineConfig.enableDepthTesting)
			{
				depthFormat = vulkanutil::chooseSupportedFormat(m_device.getPhysicalDevice(),
					{ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT },
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
			}

			// Renderpass specific resources
			curRenderNode.renderPass = VulkanRenderPass(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getImageFormat(), curRenderGraphNode.renderPass, description.renderAttachments, depthFormat);
			curRenderNode.renderFullscreenPass = curRenderGraphNode.renderPass.renderFullscreenPass;
			curRenderNode.descriptorSetLayout = VulkanDescriptorSetLayout(m_device.getLogicalDevice(), curRenderGraphNode.descriptorSetConfigs);
			curRenderNode.pushConstant = VulkanPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstantObject)); //TODO
			curRenderNode.pipeline = VulkanPipeline(m_device.getLogicalDevice(), curRenderGraphNode.pipelineConfig, curRenderNode.renderPass.getRenderPass(), curRenderNode.pushConstant.getRange(), curRenderNode.descriptorSetLayout.getDescriptorSetLayouts());
			curRenderNode.useDepthBuffer = curRenderGraphNode.pipelineConfig.enableDepthTesting;
			curDataNode.frameBufferImageViews.resize(m_swapchain.getSwapchainImagesSize());

			// The last node is always rendering to the swapchain image (thereby showing the result on the screen). Otherwise render to attachements and reference them.
			if (nodeIndex != nodeCount - 1)
			{
				for (size_t attachmentHandle : curRenderGraphNode.renderPass.outputAttachmentHandles)
				{
					const RenderAttachment attachment = description.renderAttachments.at(attachmentHandle);
					createFramebufferAttachment(curDataNode, attachmentHandle, static_cast<VkFormat>(attachment.format), VkExtent2D{ framebufferSize.width, framebufferSize.height }, 
						VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
				}
			}
			else
			{
				for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
				{
					curDataNode.frameBufferImageViews[i].addImageView(m_device.getLogicalDevice(), m_swapchain.getSwapchainImages()[i], m_swapchain.getImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
				}
			}

			// Create Depth Image and ImageView
			if (curRenderGraphNode.pipelineConfig.enableDepthTesting)
			{
				size_t depthAttachmentHandle = curRenderGraphNode.renderPass.depthStencilAttachmentHandle;
				createFramebufferAttachment(curDataNode, depthAttachmentHandle, depthFormat, m_swapchain.getSwapchainExtent(),
					VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
			}

			curRenderNode.framebuffers = VulkanFramebuffers(m_device.getLogicalDevice(), curDataNode.frameBufferImageViews, curRenderNode.renderPass.getRenderPass(), VkExtent2D{ framebufferSize.width, framebufferSize.height });
			
			// Load the meshes from RAM to the Graphics card
			loadTexturedMeshes(curDataNode, curRenderNode, curRenderGraphNode, registry, description.resourceAttachments); 

			// Bind UBOs
			for (const DescriptorSetConfig& descriptorConfig : curRenderGraphNode.descriptorSetConfigs)
			{
				for (const DescriptorBindingConfig& bindingConfig : descriptorConfig.bindings)
				{
					if (bindingConfig.type == DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					{
						size_t uboHandle = bindingConfig.inputAttachmentHandle;
						ResourceAttachment uboAttachment = description.resourceAttachments.at(uboHandle);
						curRenderNode.uniformBuffers.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), uboAttachment.dataInfo.size, maxFramesInFlight_, uboAttachment);
					}
				}
			}

			curRenderNode.descriptorSet = VulkanDescriptorSet(m_device.getLogicalDevice(), curRenderNode.descriptorSetLayout.getDescriptorSetLayouts(), curRenderNode.uniformBuffers,
				maxFramesInFlight_, curRenderGraphNode.descriptorSetConfigs, curDataNode.textureViews.getImageViews(), m_textureSampler.getImageSampler(), m_framebufferSampler.getImageSampler(), framebufferImageViewsReference, curDataNode.frameBufferImageViews);
		}

		m_imgui = VulkanImguiIntegration(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_device.getSurface(), m_instance.getVkInstance(), renderNodes.back().renderPass.getRenderPass(), m_device.getGraphicsQueue());
		m_renderFinished = VulkanSemaphore(m_device.getLogicalDevice(), m_swapchain.getSwapchainImagesSize());
		m_imageAvailable = VulkanSemaphore(m_device.getLogicalDevice(), maxFramesInFlight_);
		m_drawFences = VulkanFence(m_device.getLogicalDevice(), maxFramesInFlight_);

	}

	void VulkanRenderAPI::draw(const Window *  window, gwa::ntity::Registry& registry)
	{
		WindowSize framebufferSize = window->getFramebufferSize();
		if (framebufferSize.width <= 0 || framebufferSize.height <= 0)
		{
			m_imgui.updatePlatform();
			return;
		}
		const std::vector<VkSemaphore>& imageAvailable = m_imageAvailable.getSemaphores();
		const std::vector<VkSemaphore>& renderFinished = m_renderFinished.getSemaphores();

		vkWaitForFences(m_device.getLogicalDevice(), 1, &m_drawFences.getFences()[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_device.getLogicalDevice(), m_swapchain.getSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			recreateSwapchain(framebufferSize);
			return;
		}
		else
		{
			assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
		}
		vkResetFences(m_device.getLogicalDevice(), 1, &m_drawFences.getFences()[currentFrame]);

		vkResetCommandBuffer(*m_graphicsCommandBuffers[currentFrame].getCommandBuffer(), 0);
		recordCommands(imageIndex, registry);

	
		// Update and Render additional Platform Windows shader.
		if (const ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			m_imgui.updatePlatform();
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;						// Nummer of semaphores to wait on
		submitInfo.pWaitSemaphores = &imageAvailable[currentFrame];			// List of Semphores to wait on
		std::array<VkPipelineStageFlags,1> waitStages = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};

		submitInfo.pWaitDstStageMask = waitStages.data();				//Stages to check semaphores
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = m_graphicsCommandBuffers[currentFrame].getCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;					// Number of semaphores to signal
		submitInfo.pSignalSemaphores = &renderFinished[imageIndex];			// Semaphores to signal when command buffer finishes

		result = vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_drawFences.getFences()[currentFrame]);
		assert(result == VK_SUCCESS);

		// 3. Present image to screen when it has signaled finished rendering
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinished[imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = m_swapchain.getSwapchainPtr();
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(m_device.getPresentationQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain(framebufferSize);
		}
		else
		{
			assert(result == VK_SUCCESS);
		}
		currentFrame = (currentFrame + 1) % maxFramesInFlight_;
	}

	void VulkanRenderAPI::recordCommands(uint32_t imageIndex, gwa::ntity::Registry& registry)
	{
		VulkanCommandBuffer& cmd = m_graphicsCommandBuffers[currentFrame];
		VkExtent2D extent = m_swapchain.getSwapchainExtent();
		cmd.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
		for (size_t i = 0; i < renderNodes.size(); ++i)
		{
			RenderNode& renderNode = renderNodes[i];

			cmd.beginRenderPass(renderNode.renderPass.getOutputAttachmentCounts(), renderNode.renderPass.getRenderPass(), extent,
				renderNode.framebuffers.getFramebuffers()[imageIndex], renderNode.useDepthBuffer);
			cmd.bindPipeline(renderNode.pipeline.getPipeline());

			for (VulkanUniformBuffers buffers : renderNode.uniformBuffers)
			{
				const void* uboData = registry.getRawComponentData(buffers.getResource().resourceHandle);
				buffers.updateUniformBuffers(m_device.getLogicalDevice(), currentFrame, buffers.getResource().dataInfo.size, uboData);
			}

			cmd.setViewport({ 0.f, 0.f,(float)extent.width, (float)extent.height, 0.f, 1.f });
			cmd.setScissor({ {0,0},extent });

			const std::vector<VkDescriptorSet>& currentFrameDescriptors = renderNode.descriptorSet.getDescriptorSets(currentFrame);
			cmd.bindDescriptorSet(static_cast<uint32_t>(currentFrameDescriptors.size()), currentFrameDescriptors.data(), renderNode.pipeline.getPipelineLayout());

			for (uint32_t entity : renderNode.meshesToRender)
			{
				MeshRenderObject const* renderObject = registry.getComponent<MeshRenderObject>(entity);
				VulkanMeshBuffers::MeshBufferData meshData = renderNode.meshBuffers.getMeshBufferData(renderObject->bufferID);
				cmd.bindVertexBuffer(meshData.vertexBuffers.data(), VulkanMeshBuffers::vertexBufferCount, renderNode.meshBuffers.getOffsets().data());
				cmd.bindIndexBuffer(meshData.indexBuffer);

				pushConstantObject = { renderObject->modelMatrix, renderObject->materialTextureIDs[0] };
				cmd.pushConstants(renderNode.pipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(pushConstantObject), &pushConstantObject);

				cmd.drawIndexed(meshData.indexCount);
			}

			if (renderNode.renderFullscreenPass)
			{
				vkCmdDraw(*cmd.getCommandBuffer(), 3, 1, 0, 0);
			}

			//Render Imgui UI
			if (i == renderNodes.size() - 1) {
				// Last iteration
				m_imgui.renderData(*cmd.getCommandBuffer());
			}
			cmd.endRenderPass();
		}
		cmd.endCommandBuffer();
	}
	void VulkanRenderAPI::recreateSwapchain(WindowSize framebufferSize)
	{
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		framebufferImageViewsReference.clear();
		framebufferImageViewsReference.resize(m_swapchain.getSwapchainImagesSize());

		for (uint32_t renderPassIndex = 0; renderPassIndex < renderNodes.size(); renderPassIndex++)
		{
			DataNode& dataNode = dataNodes[renderPassIndex];
			RenderNode& renderNode = renderNodes[renderPassIndex];
			for (VulkanImageViewCollection imageViews: dataNode.frameBufferImageViews)
			{
				imageViews.cleanup(m_device.getLogicalDevice());
			}
			for (VulkanImage image : dataNode.framebufferImages)
			{
				image.cleanup(m_device.getLogicalDevice());
			}
			dataNode.framebufferImages.clear();
			dataNode.frameBufferImageViews.clear();
			dataNode.frameBufferImageViews.resize(m_swapchain.getSwapchainImagesSize());

			m_swapchain.recreateSwapchain(&m_device, framebufferSize.width, framebufferSize.height);

			if (renderPassIndex == renderNodes.size() - 1)
			{
				for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
				{
					dataNode.frameBufferImageViews[i].addImageView(m_device.getLogicalDevice(), m_swapchain.getSwapchainImages()[i], m_swapchain.getImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
				}
			}
			for (size_t renderAttachmentHandle : dataNode.renderAttachmentHandles)
			{
				RenderAttachment renderAttachment = attachmentInfos.at(renderAttachmentHandle);
				VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
				VkImageUsageFlags usageFlag = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				VkFormat format = static_cast<VkFormat>(renderAttachment.format);
				if (renderAttachment.format == Format::FORMAT_DEPTH_FORMAT)
				{
					aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
					usageFlag = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
					format = depthFormat;
				}

				dataNode.framebufferImages.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), framebufferSize.width, framebufferSize.height, format,
					VK_IMAGE_TILING_OPTIMAL, usageFlag, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
				{
					dataNode.frameBufferImageViews[i].addImageView(m_device.getLogicalDevice(), dataNode.framebufferImages.back().getImage(), format, aspectFlag);
					framebufferImageViewsReference[i].emplace(renderAttachmentHandle, dataNode.frameBufferImageViews[i].getImageViews().back());
				}
			}
			renderNode.descriptorSet.updateAttachmentReferences(m_device.getLogicalDevice(), framebufferImageViewsReference, m_framebufferSampler.getImageSampler());

			renderNode.framebuffers.recreateFramebuffer(m_device.getLogicalDevice(), dataNode.frameBufferImageViews, renderNode.renderPass.getRenderPass(), m_swapchain.getSwapchainExtent());
		}
	}

	void VulkanRenderAPI::loadTexturedMeshes(DataNode& curDataNode, RenderNode& curRenderNode, const RenderGraphNode& curRenderGraphNode, gwa::ntity::Registry& registry, const std::map<size_t, ResourceAttachment>& resourceAttachments) const
	{
		curRenderNode.meshBuffers = VulkanMeshBuffers();
		curDataNode.meshBuffersMemory = VulkanMeshBufferMemory();

		std::unordered_map<uint32_t, uint32_t>entityToIndexMap;
		uint32_t index = 1;
		size_t numberOfTextures = 1;
		Texture defaultTexture = TextureReader::loadTexture("./assets/defaultTexture.png");
		curDataNode.textures.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_device.getGraphicsQueue(), defaultTexture, m_graphicsCommandPool.getCommandPool());
		curDataNode.textureViews.addImageView(m_device.getLogicalDevice(), curDataNode.textures[0].getTextureImage().getImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		for (size_t meshKey : curRenderGraphNode.texturedMeshHandles)
		{
			const ResourceAttachment gltfMeshAttachment = resourceAttachments.at(meshKey);
			assert(gltfMeshAttachment.type == ResourceAttachmentType::ATTACHMENT_TYPE_TEXTURED_MESH);

			const GltfEntityContainer* gltfEntity = registry.getFromComponentHandle<GltfEntityContainer>(gltfMeshAttachment.resourceHandle);

			numberOfTextures += gltfEntity->textures.size();
			curDataNode.textures.resize(numberOfTextures);
			for (uint32_t textureEntity : gltfEntity->textures)
			{
				entityToIndexMap[textureEntity] = index;
				Texture const* texture = registry.getComponent<Texture>(textureEntity);
				TextureImage textureImage = TextureImage(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_device.getGraphicsQueue(), *texture, m_graphicsCommandPool.getCommandPool());
				curDataNode.textures[index] = textureImage;
				curDataNode.textureViews.addImageView(m_device.getLogicalDevice(), curDataNode.textures[index].getTextureImage().getImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
				index++;
				registry.deleteEntity(textureEntity);
			}

			for (uint32_t meshBufferEntity : gltfEntity->meshBufferEntities)
			{
				MeshBufferMemory const* meshBufferMemory = registry.getComponent<MeshBufferMemory>(meshBufferEntity);
				const uint32_t id = curRenderNode.meshBuffers.addBuffer(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), curDataNode.meshBuffersMemory, meshBufferMemory->vertices, meshBufferMemory->normals, meshBufferMemory->texcoords, meshBufferMemory->indices, m_device.getGraphicsQueue(), m_graphicsCommandPool.getCommandPool());
				uint32_t entityID = registry.registerEntity();
				MeshRenderObject renderObject;

				for (size_t i = 0; i < meshBufferMemory->materialTextureEntities.size(); i++) {
					if (meshBufferMemory->materialTextureEntities[i] != UNDEFINED_TEXTURE)
					{
						renderObject.materialTextureIDs[i] = entityToIndexMap.at(meshBufferMemory->materialTextureEntities[i]);
					}
					else
					{
						renderObject.materialTextureIDs[i] = 0;
					}
				}
				renderObject.bufferID = id;
				renderObject.modelMatrix = meshBufferMemory->modelMatrix;
				registry.emplace<MeshRenderObject>(entityID, std::move(renderObject));
				curRenderNode.meshesToRender.push_back(entityID);
				registry.deleteEntity(meshBufferEntity);
			}
		}
		registry.flushComponents<MeshBufferMemory>();
		registry.flushComponents<Texture>();
	}

	void VulkanRenderAPI::createFramebufferAttachment(DataNode& curDataNode, size_t attachmentHandle, VkFormat format, VkExtent2D attachmentSize, VkImageAspectFlagBits aspectFlag, VkImageUsageFlags usageBits)
	{
		assert(m_swapchain.getSwapchainImagesSize() == curDataNode.frameBufferImageViews.size() && m_swapchain.getSwapchainImagesSize() == framebufferImageViewsReference.size());
		curDataNode.renderAttachmentHandles.push_back(attachmentHandle);
		curDataNode.framebufferImages.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), attachmentSize.width, attachmentSize.height, format,
			VK_IMAGE_TILING_OPTIMAL, usageBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
		{
			curDataNode.frameBufferImageViews[i].addImageView(m_device.getLogicalDevice(), curDataNode.framebufferImages.back().getImage(), format, aspectFlag);
			framebufferImageViewsReference[i].emplace(attachmentHandle, curDataNode.frameBufferImageViews[i].getImageViews().back());
		}
	}

	void VulkanRenderAPI::shutdown() {
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		m_imgui.cleanup(m_device.getLogicalDevice());


		m_framebufferSampler.cleanup();
		m_textureSampler.cleanup();
		m_drawFences.cleanup();
		m_renderFinished.cleanup();
		
		m_imageAvailable.cleanup();

		m_graphicsCommandPool.cleanup();
		for (RenderNode node : renderNodes)
		{
			node.descriptorSet.cleanup(m_device.getLogicalDevice());
			node.pipeline.cleanup(m_device.getLogicalDevice());
			node.descriptorSetLayout.cleanup(m_device.getLogicalDevice());
			node.renderPass.cleanup(m_device.getLogicalDevice());
			node.meshBuffers.cleanup(m_device.getLogicalDevice());
			for (VulkanUniformBuffers buffers : node.uniformBuffers)
			{
				buffers.cleanup(m_device.getLogicalDevice());
			}
			node.framebuffers.cleanup(m_device.getLogicalDevice());
		}

		for (DataNode node : dataNodes)
		{
			node.textureViews.cleanup(m_device.getLogicalDevice());
			for (TextureImage texture : node.textures)
				texture.cleanup(m_device.getLogicalDevice());
			node.meshBuffersMemory.cleanup(m_device.getLogicalDevice());
			for (VulkanImage vulkanImage : node.framebufferImages)
				vulkanImage.cleanup(m_device.getLogicalDevice());
			for (VulkanImageViewCollection imageViews : node.frameBufferImageViews)
				imageViews.cleanup(m_device.getLogicalDevice());

		}

		m_swapchain.cleanup(m_device.getLogicalDevice());
		m_device.cleanup();
		m_instance.cleanup();
	}

}
