#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <fstream>
#include "gltfImporter.h"
#include <iostream>
#include "FileReader.h"
#include <span>
namespace gwa
{
	
	bool gltfImporter::loadResource(const ResourceManager& resourceManager, const std::filesystem::path& assetDirectory, 
		const std::string& gltfFileName, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices)
	{
		cgltf_options options = {};
		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&options, (assetDirectory/gltfFileName).string().c_str(), &data);

		if (result == cgltf_result_success)
		{
			// Read buffers
			std::vector<std::vector<std::byte>> bufferData;
			bufferData.resize(data->buffers_count);
			for (int i =0; i < data->buffers_count; ++i)
			{
				const std::filesystem::path bufferPath = assetDirectory / data->buffers[0].uri;
				bufferData[i] = FileReader::readBinaryFile<std::byte>(bufferPath);
			}

			for (int meshIndex = 0 ; meshIndex < data->meshes_count; ++meshIndex)
			{
				cgltf_mesh curMesh = data->meshes[meshIndex];
				for (int primitiveIndex = 0; primitiveIndex < curMesh.primitives_count; ++primitiveIndex)
				{
					cgltf_primitive curPrimitive = curMesh.primitives[primitiveIndex];
					if (curPrimitive.indices != nullptr)
					{
						convertToVector<uint32_t, uint32_t>(bufferData[0], indices, curPrimitive.indices->count, curPrimitive.indices->buffer_view->offset);
					}
					for (int attributeIndex = 0; attributeIndex < curPrimitive.attributes_count; ++attributeIndex)
					{
						cgltf_attribute curAttribute = curPrimitive.attributes[attributeIndex];
						if (curAttribute.type == cgltf_attribute_type_position)
						{
							convertToVector<glm::vec3, glm::vec3>(bufferData[0], vertices, curAttribute.data->count, curAttribute.data->buffer_view->offset);
						}
					}
				}
			}
			cgltf_free(data);
		}
		return false;
	}
}