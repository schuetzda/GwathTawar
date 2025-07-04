#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <memory> 
#include <vector>
#include <array>

namespace gwa
{
    constexpr uint32_t UNDEFINED_TEXTURE = std::numeric_limits<uint32_t>().max();
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

        Texture(Texture&& other) noexcept
            : width(other.width), height(other.height), pixels(std::move(other.pixels)) {
        }

        Texture& operator=(Texture&& other) noexcept {
            if (this != &other) {
                width = other.width;
                height = other.height;
                pixels = std::move(other.pixels);
            }
            return *this;
        }

        Texture& operator=(const Texture& a)
        {
            if (this != &a)             
            {
                width = a.width;
                height = a.height;
                size_t pixelCount = width * height * 4; 
                if (a.pixels)                
                {
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

    struct MeshBufferMemory
    {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        glm::mat4 modelMatrix;
        std::array<uint32_t, 3> materialTextureEntities{UNDEFINED_TEXTURE , UNDEFINED_TEXTURE, UNDEFINED_TEXTURE};
        MeshBufferMemory(size_t verticesCount, size_t indicesCount)
            : vertices(std::vector<glm::vec3>(verticesCount)),
            indices(std::vector<uint32_t>(indicesCount)),
            normals(std::vector<glm::vec3>(verticesCount)),
            texcoords(std::vector<glm::vec2>(verticesCount))
        {}
    };

    struct GltfEntityContainer
    {
        std::vector<uint32_t> meshBufferEntities;
        std::vector<uint32_t> textures;
    };

    struct MeshRenderObject
	{
		glm::mat4 modelMatrix;
        uint32_t bufferID;
        std::array<uint32_t, 3> materialTextureIDs{UNDEFINED_TEXTURE, UNDEFINED_TEXTURE, UNDEFINED_TEXTURE };
	};

    struct RenderPointLight
    {
        glm::vec4 position;
        glm::vec3 color;
        float radius;
    };

    struct LightInformation 
    {
        static constexpr uint32_t numberOfLights = 30;
        std::array<RenderPointLight, 30> lights;
        glm::vec3 viewPos;
    };
}
