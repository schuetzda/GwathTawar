#include "Window.h"
#include <cassert>
#include <GLFW/glfw3.h>


namespace gwa
{
	static inline GLFWwindow* window;

	void Window::update() const
	{
		glfwPollEvents();
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(window);
	}
	void Window::shutDown() const
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	float Window::getTime() const
	{
		return static_cast<float>(glfwGetTime());
	}
	const std::string& Window::getAppTitle() const
	{
		return appTitle_;
	}
	WindowSize Window::getFramebufferSize() const
	{
		int framebufferWidth = 0;
		int framebufferHeight = 0;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		return WindowSize{ static_cast<uint32_t>(framebufferWidth), static_cast<uint32_t>(framebufferHeight)};
	}

	void* Window::getWindowHandle() const
	{
		return window;
	}

	void Window::init(uint32_t width, uint32_t height) const
	{
		int success = glfwInit();
		assert(success);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, appTitle_.c_str(), nullptr, nullptr);
	}
	bool Window::isMousePressed(int32_t button) const
	{
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	bool Window::isKeyPressed(int32_t button) const
	{
		auto state = glfwGetKey(window, button);
		return state == GLFW_PRESS;
	}
	glm::vec2 Window::getMousePosition() const
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return glm::vec2(xpos, ypos);
	}
}
