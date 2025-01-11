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
		m_game->init(resourceManager);
		renderAPI->init(&m_window, resourceManager);
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
			m_game->run(timestep, resourceManager);
			renderAPI->uboViewProj.view = camera.getViewMatrix();
			renderAPI->draw(&m_window, resourceManager.getRenderObjects());
		}
		m_game->shutdown();
		m_window.shutDown();
	}
}