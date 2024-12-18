#include "Renderer.h"
namespace gwa {

	std::unique_ptr<RenderAPI> Renderer::s_renderAPI = RenderAPI::Create();
	gwa::Renderer::Renderer() = default;

	void Renderer::init(Window* window) const
	{
		s_renderAPI->init(window);
	}
	void Renderer::run(Window* window) const
	{
		s_renderAPI->draw(window);
	}
	void Renderer::shutdown() const 
	{
		s_renderAPI->shutdown();
	}
}
