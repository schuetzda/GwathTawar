#pragma once
#include <core/Window.h>
#include <renderer/RenderAPI.h>
#include "renderer/QuaternionCamera.h"
namespace gwa
{
	class Renderer
	{
	public:
		Renderer();
		void init(const Window* window) const;
		void run(const Window* window) ;
		void shutdown() const;

	private:
		static std::unique_ptr<RenderAPI> s_renderAPI;
		float angle = 0.f;
		float deltaTime = 0.f;
		float lasttime = 0.f;
		QuaternionCamera camera;
	};
}
