#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <memory> 
#include <vector>
#include <array>
namespace gwa
{
    struct Texture
    {
        uint32_t width{};
        uint32_t height{};
        std::unique_ptr<uint8_t> pixels{};
        
        Texture() = default;
        Texture(uint32_t imageWidth, uint32_t imageHeight, uint8_t* pixelPtr) :width(imageWidth), height(imageHeight), pixels(std::unique_ptr<uint8_t>(pixelPtr))
        {

        }
    };

    struct TexturedMeshBufferMemory
    {
        std::unique_ptr<std::vector<glm::vec3>> vertices;
        std::unique_ptr<std::vector<uint32_t>> indices;
        std::unique_ptr<std::vector<glm::vec3>> normals;
        std::unique_ptr<std::vector<glm::vec2>> texcoords;
        std::array<Texture,2> materialTextures;
        TexturedMeshBufferMemory(size_t verticesCount, size_t indicesCount)
            : vertices(std::make_unique<std::vector<glm::vec3>>(verticesCount)),
            indices(std::make_unique<std::vector<uint32_t>>(indicesCount)),
            normals(std::make_unique<std::vector<glm::vec3>>(verticesCount)),
            texcoords(std::make_unique<std::vector<glm::vec2>>(verticesCount))
        {}
    };

    
    struct Material
    {
        std::array<Texture,2> materialTextures;
        std::unique_ptr<TexturedMeshBufferMemory> uploadObjects;
        uint32_t numberOfRenderObjects;
    };

    struct Shader
    {
        std::unique_ptr<Material> materials;
        uint32_t numberOfMaterials;
    };

	struct TexturedMeshRenderObject
	{
		glm::mat4 modelMatrix;
        uint32_t bufferID;
	};
}
