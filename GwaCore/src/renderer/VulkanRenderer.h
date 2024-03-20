#pragma once
#include <Core/Window.h>

namespace gwa {

	class VulkanRenderer
	{
	public:
		explicit VulkanRenderer(Window* windowHandle);

		void init() const;
		void draw();
		void shutdown();

	private:
		Window* m_window;
	};
}

