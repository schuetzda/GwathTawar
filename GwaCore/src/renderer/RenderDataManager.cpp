#include "RenderDataManager.h"
namespace gwa
{
	void RenderDataManager::addModelData(std::span<Vertex> vertices, std::span<uint32_t> indices, Mesh* meshToBind)
	{
		RenderData renderData(vertices, indices);
		renderDataToSubmit_.push_back(renderData);
		meshesToBind_.push_back(meshToBind);
	}
}
