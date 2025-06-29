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

#ifdef GWA_DEBUG
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#else
		const std::vector<const char*> validationLayers;
#endif

		// Create Vulkan instance and enable/ disable ValidationLayers
		m_instance = VulkanInstance(window->getAppTitle(), std::string("Gwa Engine"), VK_MAKE_API_VERSION(1, 3, 0, 0),
			VK_MAKE_API_VERSION(0, 1, 0, 0), VK_API_VERSION_1_3, &validationLayers);

		m_device = VulkanDevice(window, m_instance.getVkInstance(), deviceExtensions);

		WindowSize framebufferSize = window->getFramebufferSize();
		m_swapchain = VulkanSwapchain(&m_device, framebufferSize.width, framebufferSize.height);

		m_graphicsCommandPool = VulkanCommandPool(&m_device);
		m_graphicsCommandBuffers = vulkanutil::initCommandBuffers(m_device.getLogicalDevice(), m_graphicsCommandPool.getCommandPool(), maxFramesInFlight_);

		const size_t nodeCount = description.graphNodes.size();
		renderNodes.resize(nodeCount);
		dataNodes.resize(nodeCount);

		m_textureSampler = VulkanImageSampler(m_device.getLogicalDevice(), m_device.getPhysicalDevice());

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

			curRenderNode.renderPass = VulkanRenderPass(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getImageFormat(), curRenderGraphNode.renderPass, description.renderAttachments, depthFormat);
			curRenderNode.descriptorSetLayout = VulkanDescriptorSetLayout(m_device.getLogicalDevice(), curRenderGraphNode.descriptorSetConfigs);
			curRenderNode.pushConstant = VulkanPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstantObject)); //TODO
			curRenderNode.pipeline = VulkanPipeline(m_device.getLogicalDevice(), curRenderGraphNode.pipelineConfig, curRenderNode.renderPass.getRenderPass(), curRenderNode.pushConstant.getRange(), curRenderNode.descriptorSetLayout.getDescriptorSetLayouts());
			curRenderNode.useDepthBuffer = curRenderGraphNode.pipelineConfig.enableDepthTesting;
			curDataNode.frameBufferImageViews.resize(m_swapchain.getSwapchainImagesSize());
			framebufferImageViewsReference.resize(m_swapchain.getSwapchainImagesSize());

			if (nodeIndex != nodeCount - 1)
			{
				for (size_t attachmentHandle : curRenderGraphNode.renderPass.outputAttachmentHandles)
				{
					const RenderAttachment attachment = description.renderAttachments.at(attachmentHandle);
					for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
					{
						curDataNode.framebufferImages.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), framebufferSize.width, framebufferSize.height, static_cast<VkFormat>(attachment.format),
							VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
						curDataNode.frameBufferImageViews[i].addImageView(m_device.getLogicalDevice(), curDataNode.framebufferImages.back().getImage(), static_cast<VkFormat>(attachment.format), VK_IMAGE_ASPECT_COLOR_BIT);
						framebufferImageViewsReference[i].emplace(attachmentHandle, curDataNode.frameBufferImageViews[i].getImageViews().back());
					}
				}
			}
			else
			{
				for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
				{
					curDataNode.frameBufferImageViews[i].addImageView(m_swapchain.getSwapchainImages()[i].imageView);
				}
			}

			if (curRenderGraphNode.pipelineConfig.enableDepthTesting)
			{
				size_t depthAttachmentHandle = curRenderGraphNode.renderPass.depthStencilAttachmentHandle;
				for (uint32_t i = 0; i < m_swapchain.getSwapchainImagesSize(); i++)
				{
					curDataNode.framebufferImages.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getSwapchainExtent().width, m_swapchain.getSwapchainExtent().height,
						depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
					curDataNode.frameBufferImageViews[i].addImageView(m_device.getLogicalDevice(), curDataNode.framebufferImages.back().getImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
					framebufferImageViewsReference[i].emplace(depthAttachmentHandle, curDataNode.frameBufferImageViews[i].getImageViews().back());
				}
			}
			curRenderNode.framebuffers = VulkanFramebuffers(m_device.getLogicalDevice(), curDataNode.frameBufferImageViews, curRenderNode.renderPass.getRenderPass(), VkExtent2D{ framebufferSize.width, framebufferSize.height });

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
				const ResourceAttachment gltfMeshAttachment = description.resourceAttachments.at(meshKey);
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
					registry.deleteEntity(meshBufferEntity);
				}
			}
			registry.flushComponents<MeshBufferMemory>();
			registry.flushComponents<Texture>();


			for (DescriptorSetConfig descriptorConfig : curRenderGraphNode.descriptorSetConfigs)
			{
				for (DescriptorBindingConfig bindingConfig : descriptorConfig.bindings)
				{
					if (bindingConfig.type == DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					{
						size_t uboHandle = curRenderGraphNode.descriptorSetConfigs[0].bindings[0].inputAttachmentHandle;
						ResourceAttachment uboAttachment = description.resourceAttachments.at(uboHandle);
						curRenderNode.uniformBuffers.emplace_back(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), uboAttachment.dataInfo.size, maxFramesInFlight_, uboAttachment);
					}
				}
			}

			curRenderNode.descriptorSet = VulkanDescriptorSet(m_device.getLogicalDevice(), curRenderNode.descriptorSetLayout.getDescriptorSetLayouts(), curRenderNode.uniformBuffers,
				maxFramesInFlight_, curRenderGraphNode.descriptorSetConfigs, curDataNode.textureViews.getImageViews(), m_textureSampler.getImageSampler(), framebufferImageViewsReference, curDataNode.frameBufferImageViews);

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
			
		//https://developer.nvidia.com/vulkan-shader-resource-binding
		VkExtent2D extent = m_swapchain.getSwapchainExtent();
		m_graphicsCommandBuffers[currentFrame].beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
		for (RenderNode renderNode : renderNodes)
		{
			m_graphicsCommandBuffers[currentFrame].beginRenderPass(renderNode.renderPass.getOutputAttachmentCounts(), renderNode.renderPass.getRenderPass(), extent,
				renderNode.framebuffers.getFramebuffers()[imageIndex], renderNode.useDepthBuffer);
			m_graphicsCommandBuffers[currentFrame].bindPipeline(renderNode.pipeline.getPipeline());

			
			for (VulkanUniformBuffers buffers : renderNode.uniformBuffers)
			{
				const void* uboData = registry.getRawComponentData(buffers.getResource().resourceHandle);
				buffers.updateUniformBuffers(m_device.getLogicalDevice(), currentFrame, buffers.getResource().dataInfo.size, uboData);
			}

			m_graphicsCommandBuffers[currentFrame].setViewport({ 0.f, 0.f,(float)extent.width, (float)extent.height, 0.f, 1.f });
			m_graphicsCommandBuffers[currentFrame].setScissor({ {0,0},extent });

			const std::vector<VkDescriptorSet>& currentFrameDescriptors = renderNode.descriptorSet.getDescriptorSets(currentFrame);

			if (renderNode.renderPass.getOutputAttachmentCounts() != 1)
			{

				m_graphicsCommandBuffers[currentFrame].bindDescriptorSet(static_cast<uint32_t>(currentFrameDescriptors.size()), currentFrameDescriptors.data(), renderNode.pipeline.getPipelineLayout());
				for (uint32_t entity : registry.getEntities<MeshRenderObject>())
				{
					MeshRenderObject const* renderObject = registry.getComponent<MeshRenderObject>(entity);
					VulkanMeshBuffers::MeshBufferData meshData = renderNode.meshBuffers.getMeshBufferData(renderObject->bufferID);
					std::array<VkDeviceSize, meshData.vertexBuffers.size()> offsets{ 0 };
					m_graphicsCommandBuffers[currentFrame].bindVertexBuffer(meshData.vertexBuffers.data(), static_cast<uint32_t>(meshData.vertexBuffers.size()), offsets.data());
					m_graphicsCommandBuffers[currentFrame].bindIndexBuffer(meshData.indexBuffer);

					pushConstantObject = { renderObject->modelMatrix, renderObject->materialTextureIDs[0] };
					m_graphicsCommandBuffers[currentFrame].pushConstants(renderNode.pipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(pushConstantObject), &pushConstantObject);

					m_graphicsCommandBuffers[currentFrame].drawIndexed(meshData.indexCount);
				}
			}
			else
			{	
				m_graphicsCommandBuffers[currentFrame].setViewport({ 0.f, 0.f, (float)extent.width, (float)extent.height, 0.f, 1.f });
				m_graphicsCommandBuffers[currentFrame].setScissor({ {0, 0}, extent });

				m_graphicsCommandBuffers[currentFrame].bindDescriptorSet(
					static_cast<uint32_t>(currentFrameDescriptors.size()),
					currentFrameDescriptors.data(),
					renderNode.pipeline.getPipelineLayout()
				);

				vkCmdDraw(*m_graphicsCommandBuffers[currentFrame].getCommandBuffer(), 3, 1, 0, 0);
			}

			//Render Imgui UI
			if (renderNode.renderPass.getOutputAttachmentCounts() == 1)
				m_imgui.renderData(*m_graphicsCommandBuffers[currentFrame].getCommandBuffer());

			m_graphicsCommandBuffers[currentFrame].endRenderPass();
		}
		m_graphicsCommandBuffers[currentFrame].endCommandBuffer();
	}
	void VulkanRenderAPI::recreateSwapchain(WindowSize framebufferSize)
	{
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		m_swapchain.recreateSwapchain(&m_device, framebufferSize.width, framebufferSize.height);
		/*m_swapchainDepthBufferImage = VulkanImage(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getSwapchainExtent().width, m_swapchain.getSwapchainExtent().height,
			depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_swapchainDepthBufferView.recreateImageView(0, m_device.getLogicalDevice(), m_swapchainDepthBufferImage.getImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		m_swapchainFramebuffers.recreateFramebuffer(m_device.getLogicalDevice(), m_swapchain.getSwapchainImages(), renderNodes[0].renderPass.getRenderPass(),
			m_swapchainDepthBufferView.getImageView(0), m_swapchain.getSwapchainExtent());
			*/
	}

	void VulkanRenderAPI::shutdown() {
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		m_imgui.cleanup(m_device.getLogicalDevice());


		
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
				texture.cleanup();
			node.meshBuffersMemory.cleanup(m_device.getLogicalDevice());
			for (VulkanImage vulkanImage : node.framebufferImages)
				vulkanImage.cleanup();
			for (VulkanImageViewCollection imageViews : node.frameBufferImageViews)
				imageViews.cleanup(m_device.getLogicalDevice());

		}

		m_swapchain.cleanup(m_device.getLogicalDevice());
		m_device.cleanup();
		m_instance.cleanup();
	}

}
