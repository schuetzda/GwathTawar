#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace gwa
{
	class Camera
	{
	public:
		Camera();
		void onUpdate();

		glm::mat4 getViewMatrix() const;
	private:
		glm::vec3 position;
		glm::quat orientation;
		float yaw;
		float pitch;
		float fov;
		float aspectRatio;
		float nearClip;
		float farClip;

		glm::vec2 previousMousePos;
	};
}
