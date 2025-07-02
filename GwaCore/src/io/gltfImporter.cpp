#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#include "TextureReader.h"

#include <fstream>
#include "gltfImporter.h"
#include <iostream>
#include "FileReader.h"
#include <span> 
#include <functional>

namespace gwa
{
		uint32_t gltfImporter::loadResource(gwa::ntity::Registry& registry, const std::filesystem::path& assetDirectory,
		const std::string& gltfFileName)
	{
		GltfEntityContainer gltfObject;

		cgltf_options options = {};
		std::unique_ptr<cgltf_data> gltfDataPtr;
		{
			cgltf_data* data = nullptr;
			cgltf_result result = cgltf_parse_file(&options, (assetDirectory / gltfFileName).string().c_str(), &data);
			gltfDataPtr = std::unique_ptr<cgltf_data>(data);
			if (result != cgltf_result_success)
			{
				cgltf_free(gltfDataPtr.get());
				std::cerr << "Couldn't parse gltf file";
				return -1;
			}
		}
		
		// Read buffers
		std::unordered_map<std::string, std::vector<std::byte>,StringViewHash, std::equal_to<>> bufferMemoryMap;
		for (uint32_t i = 0; i < gltfDataPtr->buffers_count; i++)
		{
			const std::filesystem::path bufferPath = assetDirectory / gltfDataPtr->buffers[i].uri;
			std::string_view key = gltfDataPtr->buffers[i].uri;
			if (!bufferMemoryMap.contains(key)) {
				bufferMemoryMap.emplace(key, FileReader::readBinaryFile<std::byte>(bufferPath));
			}
		}

		std::unordered_map<std::string, uint32_t, StringViewHash, std::equal_to<>> textureEntityMap;
		const uint32_t texturesCount = static_cast<uint32_t>(gltfDataPtr->textures_count);
		for (uint32_t i = 0; i < gltfDataPtr->textures_count; i++)
		{
			std::string_view key = gltfDataPtr->textures[i].image->uri;
			const std::filesystem::path texturePath = assetDirectory / gltfDataPtr->textures[i].image->uri;
			if (!textureEntityMap.contains(key))
			{
				uint32_t entity = registry.registerEntity();
				registry.emplace<Texture>(entity, std::move(TextureReader::loadTexture(texturePath.string().c_str())));
				textureEntityMap.emplace(key, entity);

				gltfObject.textures.push_back(entity);
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
						//assert(texcoordAttributeIndex == -1);
						texcoordAttributeIndex = attributeIndex;
						break;
					default:
						//std::cerr << " Primitive type: " << curPrimitive.attributes[attributeIndex].name << " not supported \n";
						break;
					}
				}
				
				//TexturedMesh
				if (containsIndices && positionAttributeIndex >= 0 && normalAttributeIndex >= 0)
				{
					MeshBufferMemory meshBufferData = MeshBufferMemory(curPrimitive.attributes[positionAttributeIndex].data->count, curPrimitive.indices->count);
					//Position
					if (curPrimitive.attributes[positionAttributeIndex].data->type == cgltf_type_vec3) {
						convertToVector<glm::vec3, glm::vec3>(bufferMemoryMap[curPrimitive.attributes[positionAttributeIndex].data->buffer_view->buffer->uri],
							meshBufferData.vertices, curPrimitive.attributes[positionAttributeIndex].data->count,
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
							meshBufferData.indices, curPrimitive.indices->count,
							curPrimitive.indices->buffer_view->offset
							+ curPrimitive.indices->offset);
					}
					else if (curPrimitive.indices->component_type == cgltf_component_type_r_16u)
					{
						convertToVector<uint_least16_t, uint32_t>(bufferMemoryMap[curPrimitive.indices->buffer_view->buffer->uri],
							meshBufferData.indices, curPrimitive.indices->count,
							curPrimitive.indices->buffer_view->offset
						+ curPrimitive.indices->offset);
					}
					else
					{
						assert(false);
					}

					// Normals
					if (normalAttributeIndex >= 0 && curPrimitive.attributes[normalAttributeIndex].data->type == cgltf_type_vec3)
					{
						convertToVector<glm::vec3, glm::vec3>(bufferMemoryMap[curPrimitive.attributes[normalAttributeIndex].data->buffer_view->buffer->uri],
							meshBufferData.normals, curPrimitive.attributes[normalAttributeIndex].data->count,
							curPrimitive.attributes[normalAttributeIndex].data->buffer_view->offset
							+ curPrimitive.attributes[normalAttributeIndex].data->offset);
					}

					// Texcoords
					if (texcoordAttributeIndex >= 0 && curPrimitive.attributes[texcoordAttributeIndex].data->type == cgltf_type_vec2)
					{
						convertToVector<glm::vec2, glm::vec2>(bufferMemoryMap[curPrimitive.attributes[texcoordAttributeIndex].data->buffer_view->buffer->uri],
							meshBufferData.texcoords, curPrimitive.attributes[texcoordAttributeIndex].data->count,
							curPrimitive.attributes[texcoordAttributeIndex].data->buffer_view->offset
							+ curPrimitive.attributes[texcoordAttributeIndex].data->offset);
					}

					//Textures
					if (curPrimitive.material && curPrimitive.material->has_pbr_metallic_roughness)
					{
						if (curPrimitive.material->pbr_metallic_roughness.base_color_texture.texture)
						{
							std::string colorTexturePath = curPrimitive.material->pbr_metallic_roughness.base_color_texture.texture->image->uri;
							if (textureEntityMap.contains(colorTexturePath))
							{
								meshBufferData.materialTextureEntities[0] = textureEntityMap[colorTexturePath];
							}
							else {
								std::cerr << "Texture " << colorTexturePath << " was not preloaded \n";
							}
						}
						if (curPrimitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture)
						{
							std::string metallicRoughnessTexturePath = curPrimitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri;
							if (textureEntityMap.contains(metallicRoughnessTexturePath))
							{
								meshBufferData.materialTextureEntities[1] = textureEntityMap[metallicRoughnessTexturePath];
							}
							else {
								std::cerr << "Texture " << metallicRoughnessTexturePath << " was not preloaded \n";
							}
						}
						if (curPrimitive.material && curPrimitive.material->normal_texture.texture)
						{
							std::string normalTexturePath = curPrimitive.material->normal_texture.texture->image->uri;
							if (textureEntityMap.contains(normalTexturePath))
							{
								meshBufferData.materialTextureEntities[2] = textureEntityMap[normalTexturePath];
							}
							else
							{
								std::cerr << "Texture " << normalTexturePath << " was not preloaded \n";
							}

						}
					}
					meshBufferData.modelMatrix = glm::mat4(1.f);
					uint32_t meshEntity = registry.registerEntity();

					registry.emplace<MeshBufferMemory>(meshEntity, std::move(meshBufferData));
					gltfObject.meshBufferEntities.emplace_back(meshEntity);

									}
			}
		}

		bufferMemoryMap.clear();
		cgltf_free(gltfDataPtr.get());
		gltfDataPtr.release();

		uint32_t gltfEntity = registry.registerEntity();
		registry.emplace<GltfEntityContainer>(gltfEntity, std::move(gltfObject));
		return gltfEntity;
	}
}