#pragma once
#include "ecs/Registry.h"
#include "renderer/rendergraph/RenderGraphData.h"
#include "QuaternionCamera.h"
namespace gwa
{
	/**
	 * @brief Abstract Game layer of the application. This has to be inherited and overwritten in the Project that will run the application.
	 */
	class Game
	{
	public:
		/**
		 * @brief Initialization stage of the engine. This will be called once when the application is started.
		 * @param registry ECS system registry to store and get components and entities
		 */
		virtual void init(gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera) = 0;

		virtual void initRenderGraph(gwa::ntity::Registry& registry, const gwa::Window& window, gwa::renderer::RenderGraphDescription& description, const gwa::QuaternionCamera& camera) = 0;

		/**
		 * @brief Render all UI related things using the Dear ImGui library.
		 */
		virtual void renderUI(float ts) = 0;

		/**
		 * @brief The run method gets called every cycle of the game loop.
		 * @param ts passed time in ms between this and the last call to run
		 * @param registry ECS system registry to store and get components and entities
		 */
		virtual void run(float ts, gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera) = 0;

		/**
		 * @brief Gets called once before the application is shut down.
		 */
		virtual void shutdown() = 0;

		virtual ~Game() = default;
	};
}
