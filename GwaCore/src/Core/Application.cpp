#include "Application.h"
#include <cassert>
#include <renderer/RenderAPI.h>
#include <iostream>
#include <imgui_impl_vulkan.h>
namespace gwa
{
	std::unique_ptr<renderer::RenderAPI> Application::renderAPI = renderer::RenderAPI::Create();
	Application::Application(const AppInfo& info, Game* game):m_window(Window(info.appTitle, info.appWidth, info.appHeight)), m_game(game),camera(info.appWidth / (float)info.appHeight)
	{
	}

	Application::~Application() = default;
	
	void Application::init()
	{
		uiOverlay.init(m_window);
		m_game->init(registry, camera);
		gwa::renderer::RenderGraphDescription renderGraphDescription{};
		m_game->initRenderGraph(registry, m_window, renderGraphDescription, camera);
		renderAPI->init(&m_window, registry, renderGraphDescription);
	}

	void Application::run() 
	{
		float lasttime = 0.f;
		while (!m_window.shouldClose())
		{
			const float time = m_window.getTime();
			const float timestep = time - lasttime;
			lasttime = time;
			
			m_window.update();
			camera.onUpdate(m_window);
			m_game->run(timestep, registry, camera);

			uiOverlay.beforeUIRender();
			m_game->renderUI(timestep);
			ImGui::InputFloat("Cam Speed", &camera.cspeed_, 0.001f);
			uiOverlay.afterUIRender();
			renderAPI->draw(&m_window, registry);
		}
		m_game->shutdown();
		uiOverlay.shutdown();
		renderAPI->shutdown();
		m_window.shutDown();
	}
}