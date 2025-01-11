#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace gwa
{
	struct WindowSize {
		uint32_t width;
		uint32_t height;
	};

	class Window
	{
		public:
		Window(const std::string& title = "GwaApp", uint32_t width = 1920, uint32_t height = 1080) :appTitle(title), width(width), height(height)
		{
			init();
		}

		void update() const;
		bool shouldClose() const;
		void shutDown() const;
		float getTime() const;
		const std::string& getAppTitle() const;
		void* getWindowHandle() const;
		bool isMousePressed(int32_t button) const;
		bool isKeyPressed(int32_t button) const;
		glm::vec2 getMousePosition() const;

		WindowSize getFramebufferSize() const;

	private:
		void init() const;

		const std::string& appTitle;

		uint32_t width;
		uint32_t height;
	};
}
