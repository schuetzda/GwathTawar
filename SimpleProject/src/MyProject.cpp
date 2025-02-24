#include "MyProject.h"
#include <iostream>
#include <filesystem>
#include <io/gltfImporter.h>
#include "ecs/Registry.h"
#include <array>


void MyProject::init(gwa::ntity::Registry& registry)
{
	std::filesystem::path assetPath("./assets/FlightHelmet");
	std::string gltfFileName("FlightHelmet.gltf");
	gwa::gltfImporter::loadResource(registry, assetPath, gltfFileName);
	
	registry.addComponentTable<uint32_t>(10);
	registry.addComponentTable<float>(5);

	for (int i = 0; i < 5; i++)
	{
		uint32_t entity = registry.registerEntity();
		registry.addComponent<uint32_t>(entity, i);
		registry.addComponent<float>(entity, i / 3.f);
	}
	for (int i = 5; i < 10; i++)
	{
		uint32_t entity = registry.registerEntity();
		registry.addComponent<uint32_t>(entity, i);
	}
	std::vector<uint32_t> entts = registry.getEntities<uint32_t, float>();
	for (int i = 0; i < entts.size(); i++)
	{
		std::cout << entts[i];
	}
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
}

void MyProject::shutdown()
{
}
