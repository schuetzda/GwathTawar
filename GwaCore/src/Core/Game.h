#pragma once
#include "resources/ResourceManager.h"
#include "ecs/Registry.h"
namespace gwa
{
	class Game
	{
	public:
		virtual void init(gwa::ntity::Registry& registry) = 0;
		virtual void run(float ts, gwa::ntity::Registry& registry) = 0;
		virtual void shutdown() = 0;

		virtual ~Game() = default;
	};
}
