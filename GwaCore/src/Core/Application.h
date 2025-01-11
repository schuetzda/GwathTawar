#pragma once
#include <cstdint>
#include <string>
#include "Window.h"
#include <memory>
#include "Layer.h"
#include <vector>
#include "Game.h"
#include <resources/ResourceManager.h>
#include "QuaternionCamera.h"
#include "renderer/RenderAPI.h"

int main(int argc, char** argv);

namespace gwa
{
	struct AppInfo
	{
		std::string appTitle;
		uint32_t appWidth;
		uint32_t appHeight;
	};
	class Application {
	public:
		explicit Application(const AppInfo& info, Game* game);
		~Application();
		void init();
		void run();
	private:
		Window m_window;
		Game* m_game;
		ResourceManager resourceManager;
		QuaternionCamera camera;
		static std::unique_ptr<RenderAPI> renderAPI;
	};
}
