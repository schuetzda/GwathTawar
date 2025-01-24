#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <memory> 
#include <vector>
namespace gwa
{
    struct TexturedMeshBufferMemory
    {
        std::unique_ptr<std::vector<glm::vec3>> vertices;
        std::unique_ptr<std::vector<uint32_t>> indices;
        std::unique_ptr<std::vector<glm::vec3>> normals;
        std::unique_ptr<std::vector<glm::vec2>> texcoords;

        TexturedMeshBufferMemory(size_t verticesCount, size_t indicesCount)
            : vertices(std::make_unique<std::vector<glm::vec3>>(verticesCount)),
            indices(std::make_unique<std::vector<uint32_t>>(indicesCount)),
            normals(std::make_unique<std::vector<glm::vec3>>(verticesCount)),
            texcoords(std::make_unique<std::vector<glm::vec2>>(verticesCount))
        {}

    };

	struct TexturedMeshRenderObject
	{
		uint32_t bufferID;
		glm::mat4 modelMatrix;
	};
}
