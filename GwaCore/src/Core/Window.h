#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace gwa
{
	/**
	 * @brief Structure representing the size of a window in pixels.
	 */
	struct WindowSize {
		uint32_t width;
		uint32_t height;
	};

	/**
	 * @brief Represents the main application window and provides functionality for interacting with it.
	 */
	class Window
	{
		public:

		Window(const std::string& title = "GwaApp", uint32_t width = 1920, uint32_t height = 1080) :appTitle_(title), width_(width), height_(height)
		{
			init();
		}

		/**
		 * @brief Updates the window state and processes events (e.g., input events).
		 */
		void update() const;

		/**
		 * @brief Checks if the window should close.
		 *
		 * @return True if the window should close, false otherwise.
		 */
		bool shouldClose() const;

		/**
		 * @brief Closes the window and releases resources.
		 */
		void shutDown() const;

		/**
		 * @brief Retrieves the current time since the window was created.
		 * @return A float representing the time in seconds.
		 */
		float getTime() const;

		/**
		 * @brief Retrieves the title of the application window.
		 *
		 * @return A reference to the window's title string.
		 */
		const std::string& getAppTitle() const;

		/**
		 * @brief Retrieves the native handle to the window.
		 * NOTE This is needed in order to create abstraction layer to the glfw library
		 * @return A void pointer to the window's native handle.
		 */
		void* getWindowHandle() const;

		/**
		 * @brief Checks if a specific mouse button is pressed.
		 *
		 * @param button The mouse button to check.
		 * @return True if the button is pressed, false otherwise.
		 */
		bool isMousePressed(int32_t button) const;

		/**
		 * @brief Checks if a specific keyboard key is pressed.
		 *
		 * @param button The key to check.
		 * @return True if the key is pressed, false otherwise.
		 */
		bool isKeyPressed(int32_t button) const;

		/**
		 * @brief Retrieves the current position of the mouse cursor.
		 *
		 * @return A 2D vector representing the mouse position in screen coordinates.
		 */
		glm::vec2 getMousePosition() const;

		/**
		 * @brief Retrieves the size of the window's framebuffer.
		 *
		 * @return A WindowSize struct containing the framebuffer width and height.
		 */
		WindowSize getFramebufferSize() const;

	private:
		/**
		* @brief Initializes the window and sets up necessary resources.
		*/
		void init() const;

		const std::string& appTitle_;

		uint32_t width_;
		uint32_t height_;
	};
}
