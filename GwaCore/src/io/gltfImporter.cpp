#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <fstream>
#include "gltfImporter.h"
#include <iostream>
#include "FileReader.h"
#include <span>
namespace gwa
{
	
	bool gltfImporter::loadResource(ResourceManager& resourceManager, const std::filesystem::path& assetDirectory, 
		const std::string& gltfFileName, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices)
	{
		cgltf_options options = {};
		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&options, (assetDirectory/gltfFileName).string().c_str(), &data);

		if (result == cgltf_result_success)
		{
			// Read buffers
			std::unordered_map<std::string, std::vector<std::byte>> bufferData;
			for (int i =0; i < data->buffers_count; ++i)
			{
				const std::filesystem::path bufferPath = assetDirectory / data->buffers[i].uri;
				bufferData[data->buffers[i].uri] = FileReader::readBinaryFile<std::byte>(bufferPath);
			}

			for (int meshIndex = 0 ; meshIndex < data->meshes_count; ++meshIndex)
			{
				cgltf_mesh curMesh = data->meshes[meshIndex];
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
						//TODO add more support
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
						}
					}
					
					//TexturedMesh
					if (containsIndices && positionAttributeIndex >= 0 && normalAttributeIndex >= 0)
					{
						TexturedMeshBufferData meshBufferData = resourceManager.createTexturedMeshBufferData(curPrimitive.attributes[positionAttributeIndex].data->count, curPrimitive.indices->count);
						
						//Position
						if (curPrimitive.attributes[positionAttributeIndex].data->type == cgltf_type_vec3)
						{
							convertToVector<glm::vec3, glm::vec3>(bufferData[curPrimitive.attributes[positionAttributeIndex].data->buffer_view->buffer->uri],
								meshBufferData.m_vertices, curPrimitive.attributes[positionAttributeIndex].data->count,
								curPrimitive.attributes[positionAttributeIndex].data->buffer_view->offset);
						}
						else
						{
							assert(false);
						}

						//Indices
						if (curPrimitive.indices->component_type == cgltf_component_type_r_32u)
						{
							convertToVector<uint32_t, uint32_t>(bufferData[curPrimitive.indices->buffer_view->buffer->uri],
								meshBufferData.m_indices, curPrimitive.indices->count,
								curPrimitive.indices->buffer_view->offset);
						}
						else if (curPrimitive.indices->component_type == cgltf_component_type_r_16u)
						{
							convertToVector<uint_least16_t, uint32_t>(bufferData[curPrimitive.indices->buffer_view->buffer->uri],
								meshBufferData.m_indices, curPrimitive.indices->count,
								curPrimitive.indices->buffer_view->offset);
						}
						else
						{
							assert(false);
						}

						// Normals
						if (curPrimitive.attributes[normalAttributeIndex].data->type == cgltf_type_vec3)
						{
							convertToVector<glm::vec3, glm::vec3>(bufferData[curPrimitive.attributes[normalAttributeIndex].data->buffer_view->buffer->uri],
								meshBufferData.m_normals, curPrimitive.attributes[normalAttributeIndex].data->count,
								curPrimitive.attributes[normalAttributeIndex].data->buffer_view->offset);
						}

						// Texcoords
						if (texcoordAttributeIndex > 0 && curPrimitive.attributes[texcoordAttributeIndex].data->type == cgltf_type_vec2)
						{
							convertToVector<glm::vec2, glm::vec2>(bufferData[curPrimitive.attributes[texcoordAttributeIndex].data->buffer_view->buffer->uri],
								meshBufferData.m_texcoords, curPrimitive.attributes[texcoordAttributeIndex].data->count,
								curPrimitive.attributes[texcoordAttributeIndex].data->buffer_view->offset);
						}

					}
				}
			}
			cgltf_free(data);
		}
		return false;
	}
}