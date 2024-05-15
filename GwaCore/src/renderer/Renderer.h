#pragma once
#include <Core/Window.h>
#include <renderer/RenderAPI.h>
namespace gwa
{
	class Renderer
	{
	public:
		Renderer();
		void init(Window* window);

	private:
		static std::unique_ptr<RenderAPI> s_renderAPI;
	};
}
