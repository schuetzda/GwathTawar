#include "RenderDataManager.h"
namespace gwa
{
	void RenderDataManager::addModelData(std::span<Vertex> vertices, std::span<uint32_t> indices, Mesh* meshToBind)
	{
		RenderData renderData(vertices, indices);
		renderDataToSubmit.push_back(renderData);
		meshesToBind.push_back(meshToBind);
	}
}
