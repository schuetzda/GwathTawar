#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <fstream>
#include "gltfImporter.h"
#include <iostream>
#include "FileReader.h"
#include <span> 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <functional>
namespace gwa
{
	Texture loadTexture(const char* path)
	{
		int texWidth{ 0 };
		int texHeight{ 0 };
		int texChannels{ 0 };
		uint8_t* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		return Texture(texWidth, texHeight, pixels);

	}
	bool gltfImporter::loadResource(gwa::ntity::Registry& registry, const std::filesystem::path& assetDirectory,
		const std::string& gltfFileName)
	{
		cgltf_options options = {};
		std::unique_ptr<cgltf_data> gltfDataPtr;
		{
			cgltf_data* data = nullptr;
			cgltf_result result = cgltf_parse_file(&options, (assetDirectory / gltfFileName).string().c_str(), &data);
			gltfDataPtr = std::unique_ptr<cgltf_data>(data);
			if (result != cgltf_result_success)
			{
				cgltf_free(gltfDataPtr.get());
				return false;
			}
		}
		
		// Read buffers
		std::unordered_map<std::string, std::vector<std::byte>,StringViewHash, std::equal_to<>> bufferMemoryMap;
		for (uint32_t i = 0; i < gltfDataPtr->buffers_count; ++i)
		{
			const std::filesystem::path bufferPath = assetDirectory / gltfDataPtr->buffers[i].uri;
			std::string_view key = gltfDataPtr->buffers[i].uri;
			if (!bufferMemoryMap.contains(key)) {
				bufferMemoryMap.emplace(key, FileReader::readBinaryFile<std::byte>(bufferPath));
			}
		}

		// Read textures
		std::vector<Texture> imageTextures(gltfDataPtr->textures_count);
		for (uint32_t i = 0; i < gltfDataPtr->textures_count; i++)
		{
			std::filesystem::path imageURI(gltfDataPtr->textures[i].image->uri);
			imageURI = assetDirectory / imageURI;
			if (!std::filesystem::exists(imageURI))
			{
				std::cerr << "Error during gltfImport! Texture path:" << imageURI.string() << " does not exist /n";
				imageTextures[i] = Texture(0, 0, nullptr);
			}
			else
			{
					
			}
		}

		
		for (int meshIndex = 0; meshIndex < gltfDataPtr->meshes_count; ++meshIndex)
		{
			cgltf_mesh curMesh = gltfDataPtr->meshes[meshIndex];
			//Currently Mesh to Primitive dependency is not stored in our data structure
			for (int primitiveIndex = 0; primitiveIndex < curMesh.primitives_count; ++primitiveIndex)
			{
				cgltf_primitive curPrimitive = curMesh.primitives[primitiveIndex];

				bool containsIndices = (curPrimitive.indices != nullptr);
				int positionAttributeIndex = -1;
				int normalAttributeIndex = -1;
				int texcoordAttributeIndex = -1;
				for (int attributeIndex = 0; attributeIndex < curPrimitive.attributes_count; ++attributeIndex)
				{
					switch (curPrimitive.attributes[attributeIndex].type)
					{
					case cgltf_attribute_type_position:
						assert(positionAttributeIndex == -1);
						positionAttributeIndex = attributeIndex;
						break;
					case cgltf_attribute_type_normal:
						assert(normalAttributeIndex == -1);
						normalAttributeIndex = attributeIndex;
						break;
					case cgltf_attribute_type_texcoord:
						assert(texcoordAttributeIndex == -1);
						texcoordAttributeIndex = attributeIndex;
						break;
					default:
						std::cerr << "Primitive type: " << curPrimitive.attributes[attributeIndex].name << " not supported \n";
						break;
					}
				}
				
				//TexturedMesh
				if (containsIndices && positionAttributeIndex >= 0 && normalAttributeIndex >= 0)
				{
					//TODO allocate properly
					TexturedMeshBufferMemory meshBufferData = TexturedMeshBufferMemory(curPrimitive.attributes[positionAttributeIndex].data->count, curPrimitive.indices->count);
					//Position
					if (curPrimitive.attributes[positionAttributeIndex].data->type == cgltf_type_vec3) {
						convertToVector<glm::vec3, glm::vec3>(bufferMemoryMap[curPrimitive.attributes[positionAttributeIndex].data->buffer_view->buffer->uri],
							*meshBufferData.vertices, curPrimitive.attributes[positionAttributeIndex].data->count,
							curPrimitive.attributes[positionAttributeIndex].data->buffer_view->offset
						+ curPrimitive.attributes[positionAttributeIndex].data->offset);
					}
					else
					{
						assert(false);
					}

					//Indices
					if (curPrimitive.indices->component_type == cgltf_component_type_r_32u)
					{
						convertToVector<uint32_t, uint32_t>(bufferMemoryMap[curPrimitive.indices->buffer_view->buffer->uri],
							*meshBufferData.indices, curPrimitive.indices->count,
							curPrimitive.indices->buffer_view->offset
							+ curPrimitive.indices->offset);
					}
					else if (curPrimitive.indices->component_type == cgltf_component_type_r_16u)
					{
						convertToVector<uint_least16_t, uint32_t>(bufferMemoryMap[curPrimitive.indices->buffer_view->buffer->uri],
							*meshBufferData.indices, curPrimitive.indices->count,
							curPrimitive.indices->buffer_view->offset
						+ curPrimitive.indices->offset);
					}
					else
					{
						assert(false);
					}

					// Normals
					if (curPrimitive.attributes[normalAttributeIndex].data->type == cgltf_type_vec3)
					{
						convertToVector<glm::vec3, glm::vec3>(bufferMemoryMap[curPrimitive.attributes[normalAttributeIndex].data->buffer_view->buffer->uri],
							*meshBufferData.normals, curPrimitive.attributes[normalAttributeIndex].data->count,
							curPrimitive.attributes[normalAttributeIndex].data->buffer_view->offset
							+ curPrimitive.attributes[normalAttributeIndex].data->offset);
					}

					// Texcoords
					if (curPrimitive.attributes[texcoordAttributeIndex].data->type == cgltf_type_vec2)
					{
						convertToVector<glm::vec2, glm::vec2>(bufferMemoryMap[curPrimitive.attributes[texcoordAttributeIndex].data->buffer_view->buffer->uri],
							*meshBufferData.texcoords, curPrimitive.attributes[texcoordAttributeIndex].data->count,
							curPrimitive.attributes[texcoordAttributeIndex].data->buffer_view->offset
							+ curPrimitive.attributes[texcoordAttributeIndex].data->offset);
					}

					//Textures
					if (curPrimitive.material->has_pbr_metallic_roughness)
					{
						if (curPrimitive.material->pbr_metallic_roughness.base_color_texture.texture)
						{
							std::filesystem::path colorTexturePath = curPrimitive.material->pbr_metallic_roughness.base_color_texture.texture->image->uri;
							meshBufferData.materialTextures[0] = loadTexture((assetDirectory / colorTexturePath).string().c_str());
						}
						if (curPrimitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture)
						{
							std::filesystem::path metallicRoughnessTexturePath = curPrimitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri;
							meshBufferData.materialTextures[1] = loadTexture((assetDirectory / metallicRoughnessTexturePath).string().c_str());
						}
					}
					
					uint32_t meshEntity = registry.registerEntity();
					registry.addComponent<TexturedMeshBufferMemory>(meshEntity, std::move(meshBufferData));
				}
			}
		}

		bufferMemoryMap.clear();
		cgltf_free(gltfDataPtr.get());
		gltfDataPtr.release();
		return true;
	}
}