#include "VulkanSurface.h"
#include "GLFW/glfw3.h"
#include <cassert>

gwa::VulkanSurface::VulkanSurface(Window * window, VkInstance& vkInstance)
{
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->getWindowHandle());
	VkResult result = glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &vkSurface);
	assert(result == VK_SUCCESS);
}

gwa::VulkanSurface::~VulkanSurface() = default;
