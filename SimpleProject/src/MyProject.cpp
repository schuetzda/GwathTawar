#include "MyProject.h"
#include <iostream>
#include <filesystem>
#include <io/gltfImporter.h>
#include "ecs/Registry.h"
#include <array>
#include <ecs/components/RenderObjects.h>
#include "imgui/imgui.h"
#include <numeric>


void MyProject::init(gwa::ntity::Registry& registry)
{
	std::filesystem::path assetPath("./assets/Sponza");
	std::string gltfFileName("Sponza.gltf");
	gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);
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
	ImGui::Text("Frametime: %f", ts);
	ImGui::Text("Average FPS: %f", fps);

	ImGui::Text("Key bindings:");
	ImGui::Text("w,a,s,d- Move camera");
	ImGui::Text("f,g- In-\\Decrease camera speed");
	ImGui::Text("q,a- Camera roll");
}

void MyProject::run(float ts, gwa::ntity::Registry& registry)
{
	glm::mat4 firstModel(1.0f);
	firstModel = glm::translate(firstModel, glm::vec3(0.0f, 0.0f, -2.5f));
	for (uint32_t entity: registry.getEntities<gwa::TexturedMeshRenderObject>())
	{
		gwa::TexturedMeshRenderObject* renderObject = registry.getComponent<gwa::TexturedMeshRenderObject>(entity);
		renderObject->modelMatrix = firstModel;
	}
}

void MyProject::shutdown()
{
}
