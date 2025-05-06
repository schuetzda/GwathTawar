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
        std::unique_ptr<uint8_t[]> pixels;
        
        Texture() = default;
        Texture(uint32_t imageWidth, uint32_t imageHeight, uint8_t* pixelPtr) :width(imageWidth), height(imageHeight), pixels(std::unique_ptr<uint8_t[]>(pixelPtr))
        {

        }
        Texture(const Texture& other) :width(other.width), height(other.height) 
        {
            if (other.pixels)
            {
                size_t pixelCount = width * height * 4; // Assuming 4 bytes per pixel (RGBA)
                pixels = std::make_unique<uint8_t[]>(pixelCount);
                std::copy(other.pixels.get(), other.pixels.get() + pixelCount, pixels.get());
            }
        }

        Texture& operator=(const Texture& a)
        {
            if (this != &a)             
            {
                width = a.width;
                height = a.height;
                size_t pixelCount = width * height * 4; 
                if (a.pixels)                {
                    pixels = std::make_unique<uint8_t[]>(pixelCount);
                    std::copy(a.pixels.get(), a.pixels.get() + pixelCount, pixels.get()); 
                }
                else
                {
                    pixels.reset();
                }
            }
            return *this;
        }
    };

    struct TexturedMeshBufferMemory
    {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::array<std::shared_ptr<Texture>,2> materialTextures;
        TexturedMeshBufferMemory(size_t verticesCount, size_t indicesCount)
            : vertices(std::vector<glm::vec3>(verticesCount)),
            indices(std::vector<uint32_t>(indicesCount)),
            normals(std::vector<glm::vec3>(verticesCount)),
            texcoords(std::vector<glm::vec2>(verticesCount))
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
