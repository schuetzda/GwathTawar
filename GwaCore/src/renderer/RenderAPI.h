#pragma once
#include <Core/Window.h>
#include <memory>
namespace gwa {

	class RenderAPI
	{
	public:
		enum class API {
			None = 0, Vulkan = 0
		};

	public:
		virtual ~RenderAPI() = default;

		virtual void init(Window* window) = 0;
		virtual void shutdown() = 0;

		static API GetAPI() { return s_API; }
		static std::unique_ptr<RenderAPI> Create();
	private:
		static API s_API;
	
	};
}
