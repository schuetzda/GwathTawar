#include "Application.h"
#include <cassert>
namespace gwa
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& title, uint32_t width = 1920, uint32_t height = 1080) : m_window(&Window(title, width, height))
	{
		// Only one instance of Application is allowed
		assert(!s_Instance);
		s_Instance = this;

		m_renderer = std::make_unique<VulkanRenderer>(m_window);
		m_renderer->init();
		
	}

	Application::~Application() = default;

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.push_back(layer);
	}

	void Application::run() const
	{
		float lasttime = 0.f;
		while (!m_window->shouldClose())
		{
			float time = m_window->getTime();
			float timestep = time - lasttime;
			lasttime = time;

			for (Layer* layer : m_layerStack)
			{
				layer->OnUpdate(timestep);
			}

			m_window->update();
		}
		m_window->shutDown();
	}
}