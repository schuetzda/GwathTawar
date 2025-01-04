#include "QuaternionCamera.h"
#include "core/Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
namespace gwa

{
	QuaternionCamera::QuaternionCamera()
	{
		position_ = glm::vec3(0.0f, 0.0f, 2.0f);
		glm::vec3 lookAt = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3  direction = lookAt - position_;
		direction = glm::normalize(direction);
		orientation_ = glm::quatLookAt(direction, glm::vec3(0, 1, 0));

		getViewMatrix();
	}
	void QuaternionCamera::onUpdate()
	{
		glm::vec2 mousePosition = Input::getMousePosition();
		if (Input::isMousePressed(0))
		{
			glm::vec2 diff = mousePosition - previousMousePos_;

			glm::quat qYaw = glm::angleAxis(diff.x*0.01f, glm::vec3(0, 1, 0));
			glm::quat qPitch = glm::angleAxis(diff.y*0.01f, glm::vec3(1, 0, 0));

			orientation_ = glm::normalize(qYaw) * glm::normalize(qPitch) * orientation_;
		}
		if (Input::isKeyPressed(87))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(0.0f, 0.0f, -1.0f)) * 1.f;
		}
		if (Input::isKeyPressed(83))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(0.0f, 0.0f, 1.0f)) * 1.f;
		}
		if (Input::isKeyPressed(65))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(-1.0f, 0.0f, 0.0f)) * 1.f;
		}
		if (Input::isKeyPressed(68))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(1.0f, 0.0f, 0.0f)) * 1.f;
		}
		previousMousePos_ = mousePosition;
	}
	glm::mat4 QuaternionCamera::getViewMatrix() const
	{
		glm::mat4 viewMatrix = glm::mat4_cast(orientation_);
		viewMatrix = glm::translate(viewMatrix, -position_);
		return viewMatrix;
	}
}
