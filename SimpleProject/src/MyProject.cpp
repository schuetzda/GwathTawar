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

void MyProject::init(gwa::ntity::Registry& registry)
{
	std::filesystem::path assetPath("./assets/Sponza");
	std::string gltfFileName("Sponza.gltf");
	gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);


	enum class test
	{
		attachment1,
		attachment2,
		attachment3,
		attachment4,
		pass1,
		pass2,
		pass3
	};

	gwa::renderer::RenderGraph<test> graph{};
	graph.addAttachment(test::attachment1, gwa::renderer::Format::FORMAT_A1R5G5B5_UNORM_PACK16,
			gwa::renderer::AttachmentLoadOp::ATTACHMENT_LOAD_OP_DONT_CARE,
			gwa::renderer::AttachmentStoreOp::ATTACHMENT_STORE_OP_MAX_ENUM,
			gwa::renderer::SampleCountFlagBits::SAMPLE_COUNT_2_BIT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_UNDEFINED,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.addAttachment(test::attachment2, gwa::renderer::Format::FORMAT_A1R5G5B5_UNORM_PACK16,
			gwa::renderer::AttachmentLoadOp::ATTACHMENT_LOAD_OP_DONT_CARE,
			gwa::renderer::AttachmentStoreOp::ATTACHMENT_STORE_OP_MAX_ENUM,
			gwa::renderer::SampleCountFlagBits::SAMPLE_COUNT_2_BIT,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_UNDEFINED,
			gwa::renderer::ImageLayout::IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.addRenderPass<2>(test::pass1, {test::attachment1, test::attachment2})
		.getRenderGraphDescription();
}

void MyProject::renderUI(float ts)
{
	ftime100 += ts;
	frameCount++;

	if (ftime100 > 0.5f)
	{
		avgFtime100 = ftime100 / frameCount;
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

void MyProject::run(float ts, gwa::ntity::Registry& registry)
{
	glm::mat4 firstModel(1.0f);
	firstModel = glm::translate(firstModel, glm::vec3(0.0f, 0.0f, -2.5f));
	for (uint32_t entity : registry.getEntities<gwa::TexturedMeshRenderObject>())
	{
		gwa::TexturedMeshRenderObject* renderObject = registry.getComponent<gwa::TexturedMeshRenderObject>(entity);
		renderObject->modelMatrix = firstModel;
	}
}

void MyProject::shutdown()
{
}
