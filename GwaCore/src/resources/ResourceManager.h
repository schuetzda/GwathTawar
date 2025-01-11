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
		uint32_t addResource(const TexturedMeshBufferData& bufferData)
		{
			texturedMeshBufferDataList_.push_back(bufferData);
			renderObjects.emplace_back();
			return renderObjects.size() - 1;
		}
		std::vector<TexturedMeshBufferData>& getTexturedMeshRenderData()
		{
			return texturedMeshBufferDataList_;
		}
		void addRenderObject(uint32_t meshIndex, uint32_t renderDataIndex)
		{
			renderObjects[renderObjects.size()-1 + renderDataIndex].bufferID = meshIndex;
		}

		void updateModel(uint32_t renderObjectID, const glm::mat4& modelMatrix)
		{
			renderObjects[renderObjectID].modelMatrix = modelMatrix;
		}
		const std::vector<TexturedMeshRenderObject>& getRenderObjects()
		{
			return renderObjects;
		}
	private:
		std::vector<TexturedMeshBufferData> texturedMeshBufferDataList_;
		std::vector<std::unique_ptr<TexturedMeshBufferMemory>> texturedMeshBufferMemoryList_;
		std::vector<TexturedMeshRenderObject> renderObjects;
		
	};
}
