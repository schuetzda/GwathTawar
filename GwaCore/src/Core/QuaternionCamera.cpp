#include "QuaternionCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include "Window.h"
#include "InputKeys.h"
namespace gwa

{
	QuaternionCamera::QuaternionCamera(float aspectRatio)
	{
		projMat = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10000.0f);
		projMat[1][1] *= -1;	// Invert the y-axis because difference between OpenGL and Vulkan standard

		position_ = glm::vec3(0.0f, 0.0f, 2.0f);
		glm::vec3 lookAt = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3  direction = lookAt - position_;
		direction = glm::normalize(direction);
		orientation_ = glm::quatLookAt(direction, glm::vec3(0, 1, 0));

		glm::vec3 eulerAngles = glm::eulerAngles(orientation_);

		pitch = eulerAngles.x;
		yaw = eulerAngles.y;
		roll = eulerAngles.z;
	}

	// NOTE: The callback here should be replaced by proper event handling with callback functions once ecs is implemented.
	void QuaternionCamera::onUpdate(const Window& window)
	{
		glm::vec2 mousePosition = window.getMousePosition();
		if (window.isMousePressed(0))
		{
			glm::vec2 diff = mousePosition - previousMousePos_;
			pitch += glm::radians(diff.y*0.5f);
			yaw += glm::radians(diff.x*0.5f);

			pitch = fmod(pitch, 2 * glm::pi<float>());
			yaw = fmod(yaw, 2 * glm::pi<float>());

			updateOrientationQuat();
		}
		if (window.isKeyPressed(GWA_KEY_E))
		{
			roll += glm::radians(0.1f);

			updateOrientationQuat();
		}
		if (window.isKeyPressed(GWA_KEY_Q))
		{
			roll -= glm::radians(0.1f);

			updateOrientationQuat();
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
	void QuaternionCamera::updateOrientationQuat()
	{
		glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
		glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
		glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0, 0, 1));

		orientation_ = glm::normalize(qRoll * qPitch * qYaw);
	}
}
