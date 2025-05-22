#include "Application.h"
#include <cassert>
#include <renderer/RenderAPI.h>
#include <iostream>
#include <imgui_impl_vulkan.h>
namespace gwa
{
	std::unique_ptr<RenderAPI> Application::renderAPI = RenderAPI::Create();
	Application::Application(const AppInfo& info, Game* game):m_window(Window(info.appTitle, info.appWidth, info.appHeight)), m_game(game)
	{
	}

	Application::~Application() = default;
	
	void Application::init()
	{
		const std::array<uint32_t, 2> componentEstimate{ 10, 10};
		registry.initComponentList<TexturedMeshBufferMemory, TexturedMeshRenderObject>(componentEstimate, 1000);
		uiOverlay.init(m_window);
		m_game->init(registry);
		renderAPI->init(&m_window, registry);
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
			m_game->run(timestep, registry);

			uiOverlay.beforeUIRender();
			m_game->renderUI(timestep);
			ImGui::InputFloat("Cam Speed", &camera.cspeed_, 0.001f);
			uiOverlay.afterUIRender();
			renderAPI->uboViewProj.view = camera.getViewMatrix();
			renderAPI->draw(&m_window, registry);
		}
		m_game->shutdown();
		uiOverlay.shutdown();
		renderAPI->shutdown();
		m_window.shutDown();
	}
}