#include "Renderer.h"
namespace gwa {

	std::unique_ptr<RenderAPI> Renderer::s_renderAPI = RenderAPI::Create();
	gwa::Renderer::Renderer() = default;

	void Renderer::init(Window* window) 
	{
		s_renderAPI->init(window);
	}
}
