#include "Application.h"
#include <cassert>
#include <renderer/RenderAPI.h>
namespace gwa
{
	std::unique_ptr<RenderAPI> Application::renderAPI = RenderAPI::Create();
	Application::Application(const AppInfo& info, Game* game):m_window(Window(info.appTitle, info.appWidth, info.appHeight)), m_game(game)
	{
	}

	Application::~Application() = default;

	void Application::init()
	{
		std::array<uint32_t, 2> componentEstimate{3, 3};
		registry.initComponentList<TexturedMeshBufferMemory, TexturedMeshRenderObject>(componentEstimate, 5);

		TexturedMeshBufferMemory tex(3, 2);
		uint32_t ent = registry.registerEntity();
		registry.addComponent<TexturedMeshBufferMemory>(ent, tex);
		uint32_t ent2 = registry.registerEntity();
		TexturedMeshBufferMemory tex2(1, 1);
		
		m_game->init(registry);
		renderAPI->init(&m_window, registry);
	}

	void Application::run() 
	{
		float lasttime = 0.f;
		while (!m_window.shouldClose())
		{
			float time = m_window.getTime();
			float timestep = time - lasttime;
			lasttime = time;
			
			m_window.update();
			camera.onUpdate(m_window);
			m_game->run(timestep, registry);
			renderAPI->uboViewProj.view = camera.getViewMatrix();
			renderAPI->draw(&m_window, registry);
		}
		m_game->shutdown();
		m_window.shutDown();
	}
}