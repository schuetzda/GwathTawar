#include "MyProject.h"
#include <iostream>
#include <filesystem>
#include <io/gltfImporter.h>
#include "ecs/Registry.h"
#include <array>


void MyProject::init(gwa::ResourceManager& resourceManager)
{
	struct Car {
		int velocity;
		glm::vec3 speed;
	};

	gwa::ntity::Registry reg;
	gwa::ntity::Registry reg2;
	std::array<uint32_t, 3> test {10, 10, 10};
	reg.initComponentList<Car, int, double>(test, 2000);
	

	std::filesystem::path assetPath("./assets/Rivendell");
	std::string gltfFileName("Rivendell.gltf");
	gwa::gltfImporter::loadResource(resourceManager, assetPath, gltfFileName);
}

void MyProject::run(float ts, gwa::ResourceManager& resourceManager)
{
	glm::mat4 firstModel(1.0f);
	firstModel = glm::translate(firstModel, glm::vec3(0.0f, 0.0f, -2.5f));
	resourceManager.updateModel(0, firstModel);
}

void MyProject::shutdown()
{
}
