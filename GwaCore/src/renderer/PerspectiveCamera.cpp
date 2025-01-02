#include "PerspectiveCamera.h"
#include "core/Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
namespace gwa

{
	Camera::Camera()
	{
		position = glm::vec3(0.0f, 0.0f, 4.0f);
		glm::vec3 lookAt = glm::vec3(3.0f, 0.0f, -1.0f);
		glm::vec3  direction = lookAt - position;
		direction = glm::normalize(direction);
		orientation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));

		getViewMatrix();
	}
	void Camera::onUpdate()
	{
		glm::vec2 mousePosition = Input::getMousePosition();
		if (Input::isMousePressed(0))
		{
			glm::vec2 diff = mousePosition - previousMousePos;

			glm::quat qYaw = glm::angleAxis(diff.x*0.01f, glm::vec3(0, 1, 0));
			glm::quat qPitch = glm::angleAxis(diff.y*0.01f, glm::vec3(1, 0, 0));

			orientation = glm::normalize(qYaw) * glm::normalize(qPitch) * orientation;
		}
		if (Input::isKeyPressed(87))
		{
			position += (glm::conjugate(orientation) * glm::vec3(0.0f, 0.0f, -1.0f)) * 0.01f;
		}
		if (Input::isKeyPressed(83))
		{
			position += (glm::conjugate(orientation) * glm::vec3(0.0f, 0.0f, 1.0f)) * 0.01f;
		}
		if (Input::isKeyPressed(65))
		{
			position += (glm::conjugate(orientation) * glm::vec3(-1.0f, 0.0f, 0.0f)) * 0.01f;
		}
		if (Input::isKeyPressed(68))
		{
			position += (glm::conjugate(orientation) * glm::vec3(1.0f, 0.0f, 0.0f)) * 0.01f;
		}
		previousMousePos = mousePosition;
		std::cout << position.x << " " << position.y << " " << position.z;
	}
	glm::mat4 Camera::getViewMatrix() const
	{
		glm::mat4 viewMatrix = glm::mat4_cast(orientation);
		viewMatrix = glm::translate(viewMatrix, -position);
		return viewMatrix;
	}
}
