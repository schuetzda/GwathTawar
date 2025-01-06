#pragma once
#include <vector>
#include "RenderObjects.h"
#include <memory>
namespace gwa
{
	class ResourceManager
	{
	public:
		// TODO Design proper Memory Allocation Class
		TexturedMeshBufferData createTexturedMeshBufferData(const size_t verticesCount, const size_t indicesCount)
		{
			texturedMeshBufferMemoryList_.push_back(std::make_unique<TexturedMeshBufferMemory>(verticesCount, indicesCount));

			TexturedMeshBufferData texturedMeshBufferData(texturedMeshBufferMemoryList_.back()->vertices, texturedMeshBufferMemoryList_.back()->indices,
				texturedMeshBufferMemoryList_.back()->normals, texturedMeshBufferMemoryList_.back()->texcoords);
			return texturedMeshBufferData;
		}
		void addResource(const TexturedMeshBufferData& bufferData)
		{
			texturedMeshBufferDataList_.push_back(bufferData);
		}
	private:
		std::vector<TexturedMeshBufferData> texturedMeshBufferDataList_;
		std::vector<std::unique_ptr<TexturedMeshBufferMemory>> texturedMeshBufferMemoryList_;
		
	};
}
