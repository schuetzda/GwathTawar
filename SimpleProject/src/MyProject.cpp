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

void MyProject::init(gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera)
{
	const std::array<uint32_t, 3> componentEstimate{ 10, 10, 1};
	registry.initComponentList<gwa::TexturedMeshBufferMemory, gwa::TexturedMeshRenderObject, glm::mat4>(componentEstimate, 1000);

	std::filesystem::path assetPath("./assets/Sponza");
	std::string gltfFileName("Sponza.gltf");
	gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);


}

void MyProject::initRenderGraph(gwa::ntity::Registry& registry, const gwa::Window& window, gwa::renderer::RenderGraphDescription& description, const gwa::QuaternionCamera& camera)
{
	glm::mat4 viewProjMat = camera.getProjMatrix() * camera.getViewMatrix();
	uboEntity = registry.registerEntity();
	registry.emplace(uboEntity, std::move(viewProjMat));

	gwa::renderer::PipelineBuilder pipelineBuilder{};
	gwa::renderer::PipelineConfig pipelineConfig =
		pipelineBuilder.addShaderModule("src/shaders/vert.spv", gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT)
		.addShaderModule("src/shaders/frag.spv", gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT)
		.addVertexInput(0, sizeof(glm::vec3), 0, 0, gwa::renderer::Format::FORMAT_R32G32B32_SFLOAT)
		.addVertexInput(1, sizeof(glm::vec3), 1, 0, gwa::renderer::Format::FORMAT_R32G32B32_SFLOAT)
		.addVertexInput(2, sizeof(glm::vec2), 2, 0, gwa::renderer::Format::FORMAT_R32G32_SFLOAT)
		.setPipelineInputAssembly(gwa::renderer::PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)
		.setViewport(0, 0, static_cast<float>(window.getFramebufferSize().width), static_cast<float>(window.getFramebufferSize().height))
		.setMSAA(true)
		.setDepthBuffering(true)
		.build();

	enum class deferred
	{
		color,
		depth,
		normal,
		position,
		output,
		projView
	};

	gwa::renderer::RenderGraph<deferred> graph{};
	gwa::ntity::ComponentHandle viewProjHandle = registry.getComponentHandle<glm::mat4>(uboEntity);

	description = graph.addRenderAttachment(deferred::color,
		gwa::renderer::Format::FORMAT_SWAPCHAIN_IMAGE_FORMAT,
		gwa::renderer::AttachmentLoadOp::ATTACHMENT_LOAD_OP_CLEAR,
		gwa::renderer::AttachmentStoreOp::ATTACHMENT_STORE_OP_STORE,
		gwa::renderer::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
		gwa::renderer::ImageLayout::IMAGE_LAYOUT_UNDEFINED,
		gwa::renderer::ImageLayout::IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.addRenderAttachment(deferred::depth,
			gwa::renderer::Format::FORMAT_DEPTH_FORMAT,
			gwa::renderer::AttachmentLoadOp::ATTACHMENT_LOAD_OP_CLEAR,
			gwa::renderer::AttachmentStoreOp::ATTACHMENT_STORE_OP_STORE,
			gwa::renderer::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_UNDEFINED,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		.addResourceAttachment(deferred::projView, gwa::renderer::ResourceAttachmentType::ATTACHMENT_TYPE_BUFFER, viewProjHandle, gwa::renderer::ResourceAttachment::DataSizeInfo{ sizeof(glm::mat4) })
		.addGraphNode()
		.addRenderPass<1>({ deferred::color }, deferred::depth)
		.addDescriptorSet(false)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER>(0, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT, deferred::projView)
		.addDescriptorSet(true)
		.addBinding<gwa::renderer::DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER>(1, gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT| gwa::renderer::ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT, static_cast<uint32_t>(registry.getComponentCount<gwa::Texture>()), 1024)
		.addPipeline(pipelineConfig)
		.createRenderGraph();
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

}

void MyProject::run(float ts, gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera)
{
	glm::mat4* viewProj = registry.getComponent<glm::mat4>(uboEntity);
	*viewProj = camera.getProjMatrix() * camera.getViewMatrix();
}

void MyProject::shutdown()
{
}
