#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "core/Window.h"
namespace gwa
{
	/**
	 * @brief A simple Quaternion camera implementation
	 */
	class QuaternionCamera
	{
	public:
		QuaternionCamera();
		
		/**
		 * @brief Updates the camera's position and orientation based on user input from the window. 
		 * NOTE Since in glfw window and input are closely coupled the data of both is retrived through the window class
		 *
		 * @param window The application window used to retrieve input data (e.g., mouse and keyboard).
		 */
		void onUpdate(const Window& window);

		/**
		 * @brief Retrieves and calculates the view matrix of the camera based on the orientation quaternion and the position.
		 *
		 * @return A 4x4 matrix representing the camera's view transformation.
		 */
		glm::mat4 getViewMatrix() const;
		float cspeed_ = 0.001f;
	private:
		void updateOrientationQuat();

		glm::vec3 position_;
		glm::quat orientation_;
		glm::vec2 previousMousePos_;
		float pitch;
		float yaw;
		float roll;
	};
}
