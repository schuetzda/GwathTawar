#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "core/Window.h"
namespace gwa
{
	class QuaternionCamera
	{
	public:
		QuaternionCamera();

		void onUpdate(const Window& window);
		glm::mat4 getViewMatrix() const;
	private:
		glm::vec3 position_;
		glm::quat orientation_;
		glm::vec2 previousMousePos_;
		const float cspeed_ = 1.f;
	};
}
