#pragma once
#include <cstdint>
#include <string>
#include "Window.h"
#include <memory>
#include <vector>
#include "Game.h"
#include "QuaternionCamera.h"
#include "renderer/RenderAPI.h"
#include "ecs/Registry.h"
#include "ecs/components/RenderObjects.h"

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
		
		/**
		*	@brief: Initializes all resources of the application that can be that are loaded into memory before the application is running.
		*/
		void init();

		/**
		* @brief The render loop of the engine.
		*/
		void run();
	private:
		Window m_window;
		Game* m_game;
		ntity::Registry registry; //The register for the ecs system of the application
		QuaternionCamera camera;
		static std::unique_ptr<RenderAPI> renderAPI; //TODO change RenderAPI to not be singleton
	};
}
