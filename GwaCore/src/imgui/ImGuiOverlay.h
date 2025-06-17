#pragma once
#include <core/Window.h>

namespace gwa
{
	class ImGuiOverlay
	{
	public:
		void init(const Window& window) const;
		void beforeUIRender() const;
		void afterUIRender() const;
		void shutdown() const;
	};
}
