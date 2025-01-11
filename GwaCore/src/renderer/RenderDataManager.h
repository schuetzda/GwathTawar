#pragma once
#include "VulkanAPI/vkTypes.h"
#include "Mesh.h"
#include <span>
#include <vector>
namespace gwa
{
	struct RenderData
	{
		RenderData(std::span<glm::vec3> vertices, std::span<uint32_t> indices):vertices(vertices), indices(indices)
		{

		}
		std::span<glm::vec3> vertices;
		std::span<uint32_t> indices;
	};
	class RenderDataManager
	{
	public:
		void addModelData(std::span<glm::vec3> vertices, std::span<uint32_t> indices, Mesh* meshToBind);
		std::vector<RenderData>& getRenderDataToSubmit() {
			return renderDataToSubmit_;
		}

		void setMeshBufferIndex(uint32_t meshIndex, uint32_t meshBufferIndex)
		{
			meshesToBind_[meshIndex]->meshBufferIndex = meshBufferIndex;
		}
	private:
		std::vector<Mesh*> meshesToBind_;
		std::vector<RenderData> renderDataToSubmit_;
	};
}
