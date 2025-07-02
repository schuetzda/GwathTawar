#include "MyProject.h"
#include <iostream>
#include <filesystem>
#include <io/gltfImporter.h>
#include "ecs/Registry.h"
#include <array>
#include <ecs/components/RenderObjects.h>
#include "imgui/imgui.h"
#include <numeric>
#include <renderer/rendergraph/RenderGraph.h>
#include <renderer/rendergraph/RenderValues.h>
#include <renderer/rendergraph/PipelineBuilder.h>
#include <numbers>
#include <glm/gtc/type_ptr.hpp>

void MyProject::init(gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera)
{
	const std::array<uint32_t, 6> componentEstimate{ 10, 10, 2, 5, 1024, 1};
	registry.initComponentList<gwa::MeshBufferMemory, gwa::MeshRenderObject, glm::mat4, gwa::GltfEntityContainer, gwa::Texture, gwa::LightInformation>(componentEstimate, 100);

	std::filesystem::path assetPath("./assets/Sponza");
	std::string gltfFileName("Sponza.gltf");
	gltfEntity = gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);
}

void MyProject::initRenderGraph(gwa::ntity::Registry& registry, const gwa::Window& window, gwa::renderer::RenderGraphDescription& description, const gwa::QuaternionCamera& camera)
{
	glm::mat4 viewProjMat = camera.getProjMatrix() * camera.getViewMatrix();
	uboEntity = registry.registerEntity();
	registry.emplace(uboEntity, std::move(viewProjMat));

	lightEntity = registry.registerEntity();
	std::array<gwa::RenderPointLight, 50> lights;
	for (gwa::RenderPointLight& light: lights)
	{
		light = gwa::RenderPointLight{ getRandomVec4(-800.f, 800.f, 0.f, 900.f, -400.f, 400.f), glm::vec3(1.f), lightRadius };
	}
	registry.emplace<gwa::LightInformation>(lightEntity, lights, camera.getPosition());
	lightsHandle = registry.getComponentHandle<gwa::LightInformation>(lightEntity);

	gwa::renderer::PipelineBuilder pipelineBuilder{};
	gwa::renderer::PipelineConfig gBufferPipelineConfig =
		pipelineBuilder.addShaderModule("src/shaders/gBuffer.vert.spv", gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT)
		.addShaderModule("src/shaders/gBuffer.frag.spv", gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT)
		.addVertexInput(0, sizeof(glm::vec3), 0, 0, gwa::renderer::Format::FORMAT_R32G32B32_SFLOAT)
		.addVertexInput(1, sizeof(glm::vec3), 1, 0, gwa::renderer::Format::FORMAT_R32G32B32_SFLOAT)
		.addVertexInput(2, sizeof(glm::vec2), 2, 0, gwa::renderer::Format::FORMAT_R32G32_SFLOAT)
		.setColorAttachmentCount(3)
		.setPipelineInputAssembly(gwa::renderer::PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)
		.setViewport(0, 0, static_cast<float>(window.getFramebufferSize().width), static_cast<float>(window.getFramebufferSize().height))
		.setMSAA(true)
		.setDepthBuffering(true)
		.build();

	gwa::renderer::PipelineConfig lightingPipeline = 
		pipelineBuilder.addShaderModule("src/shaders/lighting.vert.spv", gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT)
		.addShaderModule("src/shaders/lighting.frag.spv", gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT)
		.setColorAttachmentCount(1)
		.setPipelineInputAssembly(gwa::renderer::PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)
		.setViewport(0, 0, static_cast<float>(window.getFramebufferSize().width), static_cast<float>(window.getFramebufferSize().height))
		.setMSAA(true)
		.setDepthBuffering(false)
		.build();

	enum class deferred
	{
		gBufferColor,
		depth,
		normal,
		position,
		lightingColor,
		output,
		projView,
		sponzaScene,
		damagedHelmet,
		lightSource
	};

	gwa::renderer::RenderGraph<deferred> graph{};
	gwa::ntity::ComponentHandle viewProjHandle = registry.getComponentHandle<glm::mat4>(uboEntity);
	const uint32_t textureCount = static_cast<uint32_t>(registry.getComponent<gwa::GltfEntityContainer>(gltfEntity)->textures.size());

	description = graph.init()
		.addRenderAttachment(deferred::gBufferColor,
			gwa::renderer::Format::FORMAT_R8G8B8A8_UNORM,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		.addRenderAttachment(deferred::normal,
			gwa::renderer::Format::FORMAT_R16G16B16A16_SFLOAT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		.addRenderAttachment(deferred::position,
			gwa::renderer::Format::FORMAT_R16G16B16A16_SFLOAT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		.addRenderAttachment(deferred::depth,
			gwa::renderer::Format::FORMAT_DEPTH_FORMAT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		.addRenderAttachment(deferred::lightingColor,
			gwa::renderer::Format::FORMAT_SWAPCHAIN_IMAGE_FORMAT)
		.addResourceAttachment(deferred::projView, gwa::renderer::ResourceAttachmentType::ATTACHMENT_TYPE_BUFFER, viewProjHandle, gwa::renderer::ResourceAttachment::DataSizeInfo{ sizeof(glm::mat4) })
		.addResourceAttachment(deferred::lightSource, gwa::renderer::ResourceAttachmentType::ATTACHMENT_TYPE_BUFFER, lightsHandle, gwa::renderer::ResourceAttachment::DataSizeInfo{sizeof(gwa::LightInformation)})
		.addResourceAttachment(deferred::sponzaScene, gwa::renderer::ResourceAttachmentType::ATTACHMENT_TYPE_TEXTURED_MESH, registry.getComponentHandle<gwa::GltfEntityContainer>(gltfEntity))
		.addGraphNode()
		.addRenderPass<3>({ deferred::gBufferColor, deferred::position, deferred::normal }, deferred::depth)
		.addDescriptorSet(false)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER>(deferred::projView, 0, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT)
		.addDescriptorSet(true)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER>(1, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT, textureCount, 1024)
		.addTexturedMesh(deferred::sponzaScene)
		.addPipeline(gBufferPipelineConfig)
		.addGraphNode()
		.addRenderPass<1>({ deferred::lightingColor })
		.addPipeline(lightingPipeline)
		.addDescriptorSet(false)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER>(deferred::gBufferColor, 0, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT, 1, 1)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER>(deferred::position, 1, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT, 1, 1)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER>(deferred::normal, 2, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT, 1, 1)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER>(deferred::lightSource, 3, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT)
		.createRenderGraph().getRenderGraph();
}


void MyProject::renderUI(float ts)
{
	ftime100 += ts;
	frameCount++;

	if (ftime100 > 0.5f)
	{
		avgFtime100 = ftime100 / static_cast<float>(frameCount);
		ftime100 = 0.f;
		frameCount = 0;
	}
	const float fps = 1.0f / avgFtime100;

	bool windowCanClose = true;
	ImGui::Begin("Debug", &windowCanClose, ImGuiWindowFlags_MenuBar);
	ImGui::Text("Frame Time: %f", ts);
	ImGui::Text("Average FPS: %f", fps);

	ImGui::Text("Key bindings:");
	ImGui::Text("w,a,s,d- Move camera");
	ImGui::Text("f,g- In-\\Decrease camera speed");
	ImGui::Text("q,e- Camera roll");

	ImGui::InputFloat("Light Radius: %f", &lightRadius, 25.f);
	ImGui::ColorEdit3("Color", glm::value_ptr(lightColor));

}

void MyProject::run(float ts, gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera)
{
	glm::mat4* viewProj = registry.getComponent<glm::mat4>(uboEntity);
	*viewProj = camera.getProjMatrix() * camera.getViewMatrix();

	angle += angularSpeed * ts;
	if (angle > 2 * std::numbers::pi)
		angle -= 2 * std::numbers::pi;

	gwa::LightInformation* lightInfo = registry.getComponent<gwa::LightInformation>(lightEntity);
	for (gwa::RenderPointLight& light: lightInfo->lights)
	{
		light.position.x += radius * cos(angle);
		light.position.z += radius * sin(angle);
		light.radius = lightRadius;
		light.color = lightColor;
	}
	lightInfo->viewPos = camera.getPosition();
}

void MyProject::shutdown()
{
}
