#pragma once
#include <core/Window.h>
#include <memory>
#include <glm/glm.hpp>
namespace gwa {

	class RenderAPI
	{
	public:
		enum class API {
			None = 0, Vulkan = 1
		};

	public:
		virtual ~RenderAPI() = default;

		virtual void init(const Window* window) = 0;
		virtual void draw(const Window* window) = 0;
		virtual void updateModel(int modelId, const glm::mat4& newModel) = 0;

		virtual void shutdown() = 0;

		static API GetAPI() { return s_API; }
		static std::unique_ptr<RenderAPI> Create();
	private:
		static API s_API;
	};
}
