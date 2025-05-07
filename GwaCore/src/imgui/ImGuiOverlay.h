#pragma once
#include <core/Window.h>

namespace gwa
{
	class ImGuiOverlay
	{
	public:
		void init(Window& window) const;
		void update() const;
		void shutdown();
	};
}
