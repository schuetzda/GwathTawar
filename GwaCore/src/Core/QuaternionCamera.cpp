#include "QuaternionCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include "Window.h"
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

	// NOTE: The callback here should be replaced by proper event handling with callback functions once ecs is implemented.
	void QuaternionCamera::onUpdate(const Window& window)
	{
		glm::vec2 mousePosition = window.getMousePosition();
		if (window.isMousePressed(0))
		{
			glm::vec2 diff = mousePosition - previousMousePos_;

			glm::quat qYaw = glm::angleAxis(diff.x*0.01f, glm::vec3(0, 1, 0));
			glm::quat qPitch = glm::angleAxis(diff.y*0.01f, glm::vec3(1, 0, 0));

			orientation_ = glm::normalize(qYaw) * glm::normalize(qPitch) * orientation_;
		}
		if (window.isKeyPressed(87))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(0.0f, 0.0f, -1.0f)) * cspeed_;
		}
		if (window.isKeyPressed(83))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(0.0f, 0.0f, 1.0f)) * cspeed_;
		}
		if (window.isKeyPressed(65))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(-1.0f, 0.0f, 0.0f)) * cspeed_;
		}
		if (window.isKeyPressed(68))
		{
			position_ += (glm::conjugate(orientation_) * glm::vec3(1.0f, 0.0f, 0.0f)) * cspeed_;
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
