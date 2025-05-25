#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderAPI.h"
#include <array>
#include <stdexcept>
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
	void VulkanRenderAPI::init(const Window *  window, gwa::ntity::Registry& registry) 
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

		enum class test
		{
			color,
			depth,
			framebufferRenderpass
		};

		gwa::renderer::RenderGraph<test> graph{};
		gwa::renderer::RenderGraphDescription description = graph.addAttachment(test::color, 
			gwa::renderer::Format::FORMAT_SWAPCHAIN_IMAGE_FORMAT,
			gwa::renderer::AttachmentLoadOp::ATTACHMENT_LOAD_OP_CLEAR,
			gwa::renderer::AttachmentStoreOp::ATTACHMENT_STORE_OP_STORE,
			gwa::renderer::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_UNDEFINED,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.addAttachment(test::depth, 
			gwa::renderer::Format::FORMAT_DEPTH_FORMAT,
			gwa::renderer::AttachmentLoadOp::ATTACHMENT_LOAD_OP_CLEAR,
			gwa::renderer::AttachmentStoreOp::ATTACHMENT_STORE_OP_STORE,
			gwa::renderer::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_UNDEFINED,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		.addRenderPass<1>(test::framebufferRenderpass, { test::color}, test::depth)
		.createRenderGraph();

		m_renderPass = VulkanRenderPass(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getImageFormat(), description);

		DescriptorSetConfigurator descriptorConfig{};
		descriptorConfig
			.addBinding(0, DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER, ShaderStage::SHADER_STAGE_VERTEX_BIT).finalizeDescriptorSet(false)
			.addBinding(1, DescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE, ShaderStage::SHADER_STAGE_FRAGMENT_BIT, 512)
			.addBinding(2, DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE, ShaderStage::SHADER_STAGE_FRAGMENT_BIT, 512).finalizeDescriptorSet(true);

		m_descriptorSetLayout = VulkanDescriptorSetLayout(m_device.getLogicalDevice());

		m_bindlessDescriptorSet = VulkanBindlessDescriptor(m_device.getLogicalDevice(),128,1,maxFramesInFlight_);

		m_pushConstant = VulkanPushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4)); //TODO

		renderer::PipelineBuilder pipelineBuilder{};
		renderer::PipelineConfig pipelineConfig =  
			pipelineBuilder.addShaderModule("src/shaders/vert.spv", renderer::ShaderStage::SHADER_STAGE_VERTEX_BIT)
			.addShaderModule("src/shaders/frag.spv", renderer::ShaderStage::SHADER_STAGE_FRAGMENT_BIT)
			.addVertexInput(0, sizeof(glm::vec3), 0, 0, renderer::Format::FORMAT_R32G32B32_SFLOAT)
			.addVertexInput(1, sizeof(glm::vec3), 1, 0, renderer::Format::FORMAT_R32G32B32_SFLOAT)
			.addVertexInput(2, sizeof(glm::vec2), 2, 0, renderer::Format::FORMAT_R32G32_SFLOAT)
			.setPipelineInputAssembly(renderer::PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)
			.setViewport(0, 0, static_cast<float>(framebufferSize.width), static_cast<float>(framebufferSize.height))
			.setMSAA(true)
			.setDepthBuffering(true)
			.build();

		m_graphicsPipeline = VulkanPipeline(m_device.getLogicalDevice(), pipelineConfig, m_renderPass.getRenderPass(), m_pushConstant.getRange(), m_descriptorSetLayout.getDescriptorSetLayout());

		m_depthBufferImage = VulkanImage(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getSwapchainExtent().width, m_swapchain.getSwapchainExtent().height,
			m_renderPass.getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_depthBufferImageView.addImageView(m_device.getLogicalDevice(), m_depthBufferImage.getImage(), m_renderPass.getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT); 

		m_swapchainFramebuffers = VulkanSwapchainFramebuffers(m_device.getLogicalDevice(), m_swapchain.getSwapchainImages(), 
			m_renderPass.getRenderPass(), m_depthBufferImageView.getImageView(0), m_swapchain.getSwapchainExtent());

		m_graphicsCommandPool = VulkanCommandPool(&m_device);
		
		m_meshBuffers = VulkanMeshBuffers(m_device.getLogicalDevice(), m_device.getPhysicalDevice());
		uboViewProj.projection = glm::perspective(glm::radians(45.0f), (float)m_swapchain.getSwapchainExtent().width / (float)m_swapchain.getSwapchainExtent().height, 0.1f, 10000.0f);

		uboViewProj.projection[1][1] *= -1;	// Invert the y-axis because difference between OpenGL and Vulkan standard

		{
			std::unordered_map<uint32_t, uint32_t>entityToIndexMap;

			uint32_t index = 0;
			const size_t numberOfTextures = registry.getComponentCount<Texture>();
			m_textures.resize(numberOfTextures);
			for (uint32_t textureEntity : registry.getEntities<Texture>())
			{
				entityToIndexMap[textureEntity] = index;
				Texture const* texture = registry.getComponent<Texture>(textureEntity);
				TextureImage textureImage = TextureImage(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_device.getGraphicsQueue(), *texture, m_graphicsCommandPool.getCommandPool());
				m_textures[index] = textureImage;
				m_textureViews.addImageView(m_device.getLogicalDevice(), m_textures[index].getTextureImage().getImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
				index++;
				registry.deleteEntity(textureEntity);
			}

			for (uint32_t meshBufferEntity: registry.getEntities<TexturedMeshBufferMemory>())
			{
				TexturedMeshBufferMemory const* meshBufferMemory = registry.getComponent<TexturedMeshBufferMemory>(meshBufferEntity);
				const uint32_t id = m_meshBuffers.addBuffer(meshBufferMemory->vertices, meshBufferMemory->normals, meshBufferMemory->texcoords, meshBufferMemory->indices, m_device.getGraphicsQueue(), m_graphicsCommandPool.getCommandPool());
				uint32_t entityID = registry.registerEntity();
				TexturedMeshRenderObject renderObject;
				
				assert(meshBufferMemory->materialTextureEntities.size() == renderObject.materialTextureIDs.size()); //TexturedMeshBufferMemory and TexturedMeshRenderObject are required to have the same amount of textures.
				for (size_t i = 0; i < meshBufferMemory->materialTextureEntities.size(); i++) {
					renderObject.materialTextureIDs[i] = entityToIndexMap.at(meshBufferMemory->materialTextureEntities[i]);
				}
				renderObject.bufferID = id;
				registry.emplace<TexturedMeshRenderObject>(entityID, std::move(renderObject));
				registry.deleteEntity(meshBufferEntity);
			}
		}
		registry.flushComponents<TexturedMeshBufferMemory>();
		registry.flushComponents<Texture>();

		m_textureSampler = VulkanImageSampler(m_device.getLogicalDevice(), m_device.getPhysicalDevice());
				
		m_graphicsCommandBuffers = vulkanutil::initCommandBuffers(m_device.getLogicalDevice(), m_graphicsCommandPool.getCommandPool(), maxFramesInFlight_);

		m_mvpUniformBuffers = VulkanUniformBuffers(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), sizeof(UboViewProj), m_swapchain.getSwapchainImagesSize());
		m_descriptorSet = VulkanDescriptorSet(m_device.getLogicalDevice(), m_descriptorSetLayout.getDescriptorSetLayout(), m_mvpUniformBuffers.getUniformBuffers(),
			maxFramesInFlight_, sizeof(UboViewProj));

		m_bindlessDescriptorSet.addTextures(m_device.getLogicalDevice(), maxFramesInFlight_, m_textureViews.getImageViews(), m_textureSampler.getImageSampler(), 1);

		m_renderFinished = VulkanSemaphore(m_device.getLogicalDevice(), m_swapchain.getSwapchainImagesSize());
		m_imageAvailable = VulkanSemaphore(m_device.getLogicalDevice(), maxFramesInFlight_);
		m_drawFences = VulkanFence(m_device.getLogicalDevice(), maxFramesInFlight_);

		m_imgui = VulkanImguiIntegration(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_device.getSurface(), m_instance.getVkInstance(), m_renderPass.getRenderPass(), m_device.getGraphicsQueue());
	}

	void VulkanRenderAPI::draw(const Window *  window, gwa::ntity::Registry& registry)
	{
		WindowSize framebufferSize = window->getFramebufferSize();
		if (framebufferSize.width <= 0 || framebufferSize.height == 0)
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

		m_mvpUniformBuffers.updateUniformBuffers(imageIndex, sizeof(UboViewProj), &uboViewProj);
		vkResetCommandBuffer(*m_graphicsCommandBuffers[currentFrame].getCommandBuffer(), 0);
		recordCommands(imageIndex, registry);

	
		// Update and Render additional Platform Windows
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

	void VulkanRenderAPI::shutdown() {
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		m_imgui.cleanup(m_device.getLogicalDevice());

		m_textureViews.cleanup(m_device.getLogicalDevice());

		for (TextureImage texture : m_textures)
			texture.cleanup();

		m_textureSampler.cleanup();
		m_meshBuffers.cleanup();
		m_drawFences.cleanup();
		m_renderFinished.cleanup();
		
		m_imageAvailable.cleanup();
		m_descriptorSet.cleanup();

		m_mvpUniformBuffers.cleanup();
		m_graphicsCommandPool.cleanup();
		m_swapchainFramebuffers.cleanup();
		m_depthBufferImageView.cleanup(m_device.getLogicalDevice());
		m_depthBufferImage.cleanup();
		m_graphicsPipeline.cleanup(m_device.getLogicalDevice());
		m_descriptorSetLayout.cleanup();
		m_renderPass.cleanup(m_device.getLogicalDevice());
		m_swapchain.cleanup();
		m_device.cleanup();
		m_instance.cleanup();
	}
	void VulkanRenderAPI::recordCommands(uint32_t imageIndex, gwa::ntity::Registry& registry)
	{
		//https://developer.nvidia.com/vulkan-shader-resource-binding
		VkExtent2D extent = m_swapchain.getSwapchainExtent();
		m_graphicsCommandBuffers[currentFrame].beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
		m_graphicsCommandBuffers[currentFrame].beginRenderPass(m_renderPass.getRenderPass(), extent,
			m_swapchainFramebuffers.getFramebuffers()[imageIndex]);
		m_graphicsCommandBuffers[currentFrame].bindPipeline(m_graphicsPipeline.getPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_graphicsCommandBuffers[currentFrame].setViewport(viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		m_graphicsCommandBuffers[currentFrame].setScissor(scissor);	
		
		for (uint32_t entity: registry.getEntities<TexturedMeshRenderObject>())
		{
			TexturedMeshRenderObject const* renderObject = registry.getComponent<TexturedMeshRenderObject>(entity);
			VulkanMeshBuffers::MeshBufferData meshData = m_meshBuffers.getMeshBufferData(renderObject->bufferID);
			constexpr uint32_t vertexBufferSize = 3;
			std::array<VkDeviceSize, vertexBufferSize> offsets = { 0, 0, 0 };
			const VkBuffer vertexBuffers[vertexBufferSize] = { meshData.vertexBuffer, meshData.normalBuffer, meshData.texcoordBuffer };
			m_graphicsCommandBuffers[currentFrame].bindVertexBuffer(vertexBuffers, static_cast<uint32_t>(offsets.size()), offsets.data());
			m_graphicsCommandBuffers[currentFrame].bindIndexBuffer(meshData.indexBuffer);

			m_graphicsCommandBuffers[currentFrame].pushConstants(m_graphicsPipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, &renderObject->modelMatrix);
			m_graphicsCommandBuffers[currentFrame].bindDescriptorSet(m_descriptorSets[renderObject->materialTextureIDs[0]].getDescriptorSets()[currentFrame], m_graphicsPipeline.getPipelineLayout());

			m_graphicsCommandBuffers[currentFrame].drawIndexed(meshData.indexCount);
		}

		//Render Imgui UI
		m_imgui.renderData(*m_graphicsCommandBuffers[currentFrame].getCommandBuffer());

		m_graphicsCommandBuffers[currentFrame].endRenderPass();
		m_graphicsCommandBuffers[currentFrame].endCommandBuffer();
	}
	void VulkanRenderAPI::recreateSwapchain(WindowSize framebufferSize)
	{
		vkDeviceWaitIdle(m_device.getLogicalDevice());
		m_swapchainFramebuffers.cleanup();
		m_depthBufferImageView.cleanup();
		m_depthBufferImage.cleanup();
		m_swapchain.recreateSwapchain(framebufferSize.width, framebufferSize.height);
		m_depthBufferImage = VulkanImage(m_device.getLogicalDevice(), m_device.getPhysicalDevice(), m_swapchain.getSwapchainExtent().width, m_swapchain.getSwapchainExtent().height,
			m_renderPass.getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthBufferImageView = VulkanImageView(m_device.getLogicalDevice(), m_depthBufferImage.getImage(), m_renderPass.getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
		m_swapchainFramebuffers.recreateSwapchain(m_swapchain.getSwapchainImages(), m_renderPass.getRenderPass(),
			m_depthBufferImageView.getImageView(), m_swapchain.getSwapchainExtent());
	}
}
