#include "MyProject.h"
#include <iostream>
#include <filesystem>
#include <io/gltfImporter.h>
#include "ecs/Registry.h"
#include <array>
#include "resources/RenderObjects.h"


void MyProject::init(gwa::ntity::Registry& registry)
{
	std::filesystem::path assetPath("./assets/Rivendell");
	std::string gltfFileName("Rivendell.gltf");
	gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);

}

void MyProject::run(float ts, gwa::ntity::Registry& registry)
{
	glm::mat4 firstModel(1.0f);
	firstModel = glm::translate(firstModel, glm::vec3(0.0f, 0.0f, -2.5f));
	for (uint32_t i = 0; i < registry.getComponentCount<gwa::TexturedMeshRenderObject>(); i++)
	{
		gwa::TexturedMeshRenderObject* renderObject = registry.getComponent<gwa::TexturedMeshRenderObject>(i);
		renderObject->modelMatrix = firstModel;
	}
	//resourceManager.updateModel(0, firstModel);
}

void MyProject::shutdown()
{
}
