#pragma once
#include <cstdint>
#include <string>
#include "Window.h"
#include <memory>
#include "Layer.h"
#include <vector>
#include "renderer/Renderer.h"
int main(int argc, char** argv);

namespace gwa
{

	class Application {
	public:
		explicit Application(const std::string& title, uint32_t width, uint32_t height);
		
		virtual ~Application();
		
		static Application& getInstance() { return *s_Instance; }

		void PushLayer(Layer* layer);
		
	private:
		Application(Application const& copy) = delete;
		Application& operator=(Application const& copy) = delete;

		void run() const;

		Window * const m_window;

		std::vector<Layer*> m_layerStack;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);

		//TODO Change scope of renderer. Decide where it is accessible.
		std::unique_ptr<Renderer> m_renderer;
	};

	std::unique_ptr<Application> createApplication();

	
}
