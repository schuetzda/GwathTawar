#include "MyProject.h"
#include <iostream>
#include <filesystem>
#include <io/gltfImporter.h>
#include "ecs/Registry.h"
#include <array>
#include <ecs/components/RenderObjects.h>


void MyProject::init(gwa::ntity::Registry& registry)
{
	std::filesystem::path assetPath("./assets/Sponza");
	std::string gltfFileName("Sponza.gltf");
	gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);
	registry.initNewComponent<uint32_t>(20000);
	for (uint32_t i = 0; i < 100000; ++i)
	{
		uint32_t entity = registry.registerEntity();
		registry.emplace(entity, i);
	}
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
