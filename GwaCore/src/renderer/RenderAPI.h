#pragma once
#include <core/Window.h>
#include <memory>
#include <glm/glm.hpp>
#include "ecs/Registry.h"
namespace gwa {

	class RenderAPI
	{
	public:
		enum class API {
			None = 0, Vulkan = 1
		};

		virtual ~RenderAPI() = default;

		virtual void init(const Window* window, gwa::ntity::Registry& registry) = 0;
		virtual void draw(const Window* window, gwa::ntity::Registry& registry) = 0;
		virtual void shutdown() = 0;

		static API GetAPI() { return s_API; }
		static std::unique_ptr<RenderAPI> Create();
		
		struct UboViewProj {
			glm::mat4 projection;
			glm::mat4 view;
		} uboViewProj;

		std::vector<uint32_t> m_meshes; //TODO move to protected
	private:
		static API s_API;
		
	};
}
