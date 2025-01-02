#pragma once
#include <glm/glm.hpp>
namespace gwa {
	class Input 
	{
	public:
		static bool isMousePressed(int32_t button);
		static bool isKeyPressed(int32_t button);
		static glm::vec2 getMousePosition();
	};
}
