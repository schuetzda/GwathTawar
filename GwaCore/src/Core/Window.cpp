#include "Window.h"
#include <cassert>
#include <GLFW/glfw3.h>


namespace gwa
{
	// TODO improve global variable
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
		return appTitle;
	}
	WindowSize Window::getFramebufferSize() const
	{
		int framebufferWidth = 0;
		int framebufferHeight = 0;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		return WindowSize{ width, height };
	}
	void* Window::getWindowHandle() const
	{
		return window;
	}
	void Window::init() const
	{
		int success = glfwInit();
		assert(success);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, appTitle.c_str(), nullptr, nullptr);
	}
}
