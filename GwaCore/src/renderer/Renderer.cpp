#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "io/gltfImporter.h"
#include <filesystem>
#include <vector>
namespace gwa {

	std::unique_ptr<RenderAPI> Renderer::s_renderAPI = RenderAPI::Create();
	gwa::Renderer::Renderer() = default;

	void Renderer::init(const Window* window) const
	{
		/*
		std::vector<Vertex> meshVertices1 = {
		{ { -0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },	// 0
		{ { -0.4, -0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },	    // 1
		{ { 0.4, -0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },    // 2
		{ { 0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },   // 3
		};

		std::vector<Vertex> meshVertices2 = {
			{ { -0.25, 0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },	// 0
			{ { -0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },	    // 1
			{ { 0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },    // 2
			{ { 0.25, 0.6, 0.0 },{ 0.0f, 0.0f, 11.0f } },   // 3
		};

		//Index Data
		std::vector<uint32_t> meshIndices = {
			0, 1, 2,
			2, 3, 0
		}; */
		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
		std::vector<Vertex> vertex;
		ResourceManager manager;
		std::filesystem::path assetPath("./assets/Rivendell");
		std::string gltfFileName("Rivendell.gltf");
		gltfImporter::loadResource(manager, assetPath, gltfFileName, vertices, indices);

		for (int i = 0; i < vertices.size(); ++i)
		{
			Vertex newVertex;
			newVertex.position = vertices[i];
			newVertex.color = glm::vec3(1.f, 0.5f, 0.5f);
			vertex.push_back(newVertex);
		}

		s_renderAPI->m_meshes.emplace_back();
		s_renderAPI->renderDataManager.addModelData(std::span<Vertex>(vertex), std::span<uint32_t>(indices), &s_renderAPI->m_meshes.back());
		s_renderAPI->init(window);
		s_renderAPI->uboViewProj.view = camera.getViewMatrix();
	}
	void Renderer::run(const Window* window) 
	{
		float now = window->getTime();
		deltaTime = now - lasttime;
		lasttime = now;

		angle += 10.f * deltaTime;
		if (angle >= 360.f)
		{
			angle = 0.f;
		}

		glm::mat4 firstModel(1.0f);
		glm::mat4 secondModel(1.0f);

		firstModel = glm::translate(firstModel, glm::vec3(0.0f, 0.0f, -2.5f));
		firstModel = glm::rotate(firstModel, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

		secondModel = glm::translate(secondModel, glm::vec3(0.0f, 0.0f, -3.0f));
		secondModel = glm::rotate(secondModel, glm::radians(-angle * 10), glm::vec3(0.0f, 1.0f, 0.0f));

		camera.onUpdate();
		s_renderAPI->uboViewProj.view = camera.getViewMatrix();

		s_renderAPI->updateModel(0, firstModel);

		s_renderAPI->draw(window);
	}
	void Renderer::shutdown() const 
	{
		s_renderAPI->shutdown();
	}
}
