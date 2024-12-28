#include <glm/glm.hpp>
#include "VulkanAPI/vkTypes.h"
#pragma once
namespace gwa
{
	class Mesh
	{
	public:
		Mesh() = default;

		uint32_t meshBufferIndex;
		glm::mat4 modelMatrix;
	};
}
