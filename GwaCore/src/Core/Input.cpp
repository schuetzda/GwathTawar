#include "Input.h"
#include <GLFW/glfw3.h>
#include "Application.h"
namespace gwa
{
	// TODO rewrite
	bool Input::isMousePressed(int32_t button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getWindowHandle());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	bool Input::isKeyPressed(int32_t button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getWindowHandle());
		auto state = glfwGetKey(window, button);
		return state == GLFW_PRESS;
	}
	glm::vec2 Input::getMousePosition()
	{
		double xpos, ypos;
		auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getWindowHandle());
		glfwGetCursorPos(window, &xpos, &ypos);
		return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
	}

}
