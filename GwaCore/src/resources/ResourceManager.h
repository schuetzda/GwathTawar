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
		TexturedMeshBufferMemory createTexturedMeshBufferData(const size_t verticesCount, const size_t indicesCount)
		{
			texturedMeshBufferMemoryList_.push_back(std::make_unique<TexturedMeshBufferMemory>(verticesCount, indicesCount));
			return TexturedMeshBufferMemory(2,3);
		}
	private:
		std::vector<std::unique_ptr<TexturedMeshBufferMemory>> texturedMeshBufferMemoryList_;
		
	};
}
