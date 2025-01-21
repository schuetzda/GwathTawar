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

        // Move constructor
        TexturedMeshBufferMemory(TexturedMeshBufferMemory&& other) noexcept
            : vertices(std::move(other.vertices)),
            indices(std::move(other.indices)),
            normals(std::move(other.normals)),
            texcoords(std::move(other.texcoords))
        {}

        // Move assignment operator
        TexturedMeshBufferMemory& operator=(TexturedMeshBufferMemory&& other) noexcept {
            if (this != &other) {
                vertices = std::move(other.vertices);
                indices = std::move(other.indices);
                normals = std::move(other.normals);
                texcoords = std::move(other.texcoords);
            }
            return *this;
        }

        // Copy constructor (optional, depending on needs)
        TexturedMeshBufferMemory(const TexturedMeshBufferMemory& other)
            : vertices(std::make_unique<std::vector<glm::vec3>>(*other.vertices)),
            indices(std::make_unique<std::vector<uint32_t>>(*other.indices)),
            normals(std::make_unique<std::vector<glm::vec3>>(*other.normals)),
            texcoords(std::make_unique<std::vector<glm::vec2>>(*other.texcoords))
        {}

        // Copy assignment operator (optional)
        TexturedMeshBufferMemory& operator=(const TexturedMeshBufferMemory& other) {
            if (this != &other) {
                vertices = std::make_unique<std::vector<glm::vec3>>(*other.vertices);
                indices = std::make_unique<std::vector<uint32_t>>(*other.indices);
                normals = std::make_unique<std::vector<glm::vec3>>(*other.normals);
                texcoords = std::make_unique<std::vector<glm::vec2>>(*other.texcoords);
            }
            return *this;
        }
    };

	struct TexturedMeshRenderObject
	{
		uint32_t bufferID;
		glm::mat4 modelMatrix;
	};
}
