#pragma once
#include "VulkanAPI/vkTypes.h"
#include "Mesh.h"
#include <span>
#include <vector>
namespace gwa
{
	class RenderData
	{
	public:
		RenderData(std::span<Vertex> vertices, std::span<uint32_t> indices):vertices(vertices), indices(indices)
		{

		}
		std::span<Vertex> vertices;
		std::span<uint32_t> indices;
	};
	class RenderDataManager
	{
	public:
		//TODO make constant
		void addModelData(std::span<Vertex> vertices, std::span<uint32_t> indices, Mesh* meshToBind);
		std::vector<RenderData>& getRenderDataToSubmit() {
			return renderDataToSubmit;
		}
		std::vector<Mesh*> meshesToBind;
	private:
		std::vector<RenderData> renderDataToSubmit;
	};
}
