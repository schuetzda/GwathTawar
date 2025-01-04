#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace gwa
{
	class QuaternionCamera
	{
	public:
		QuaternionCamera();
		void onUpdate();

		glm::mat4 getViewMatrix() const;
	private:
		glm::vec3 position_;
		glm::quat orientation_;
		glm::vec2 previousMousePos_;
	};
}
