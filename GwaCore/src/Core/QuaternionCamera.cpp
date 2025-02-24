#include "QuaternionCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include "Window.h"
#include "InputKeys.h"
namespace gwa

{
	QuaternionCamera::QuaternionCamera()
	{
		position_ = glm::vec3(0.0f, 0.0f, 2.0f);
		glm::vec3 lookAt = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3  direction = lookAt - position_;
		direction = glm::normalize(direction);
		orientation_ = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
		qPitch = glm::angleAxis(0.f, glm::vec3(1, 0, 0));
		qYaw = glm::angleAxis(0.f, glm::vec3(0, 1, 0));
		qRoll = glm::angleAxis(0.f, glm::vec3(0, 0, 1));

		getViewMatrix();
	}

	// NOTE: The callback here should be replaced by proper event handling with callback functions once ecs is implemented.
	void QuaternionCamera::onUpdate(const Window& window)
	{
		glm::vec2 mousePosition = window.getMousePosition();
		if (window.isMousePressed(0))
		{
			glm::vec2 diff = mousePosition - previousMousePos_;

			qPitch = qPitch * glm::angleAxis(diff.y*0.01f, glm::vec3(1, 0, 0));
		    qYaw = qYaw * glm::angleAxis(diff.x*0.01f, glm::vec3(0, 1, 0));

			orientation_ =  glm::normalize(qPitch) * glm::normalize(qYaw) * glm::normalize(qRoll);
		}
		if (window.isKeyPressed(GWA_KEY_E))
		{
			qRoll = qRoll * glm::angleAxis(0.01f, glm::vec3(0, 0, 1));
			orientation_ =  glm::normalize(qPitch) * glm::normalize(qYaw) * glm::normalize(qRoll);
		}
		if (window.isKeyPressed(GWA_KEY_Q))
		{
			qRoll = qRoll * glm::angleAxis(-0.01f, glm::vec3(0, 0, 1));
			orientation_ =  glm::normalize(qPitch) * glm::normalize(qYaw) * glm::normalize(qRoll);
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
		if (window.isKeyPressed(GWA_KEY_F))
		{
			cspeed_ += 0.001f;
		}
		if (window.isKeyPressed(GWA_KEY_G))
		{
			cspeed_ -= 0.001f;
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
