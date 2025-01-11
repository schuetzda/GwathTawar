#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <renderer/VulkanAPI/vkTypes.h>
#include <vector>
namespace gwa
{
	struct TexturedMeshBufferData
	{
		std::vector<glm::vec3>& m_vertices;
		std::vector<uint32_t>& m_indices;
		std::vector<glm::vec3>& m_normals;
		std::vector<glm::vec2>& m_texcoords;

		TexturedMeshBufferData(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texcoords)
			:m_vertices(vertices), m_indices(indices), m_normals(normals), m_texcoords(texcoords)
		{}
	};

	struct TexturedMeshBufferMemory
	{
		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;

		TexturedMeshBufferMemory(size_t verticesCount, size_t indicesCount): vertices(std::vector<glm::vec3>(verticesCount)), indices(std::vector<uint32_t>(indicesCount)),
			normals(std::vector<glm::vec3>(verticesCount)), texcoords(std::vector<glm::vec2>(verticesCount))
		{}
	};

	struct TexturedMeshRenderObject
	{
		uint32_t bufferID;
		glm::mat4 modelMatrix;
	};
}
