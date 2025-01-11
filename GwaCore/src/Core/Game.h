#pragma once
#include "resources/ResourceManager.h"
namespace gwa
{
	class Game
	{
	public:
		virtual void init(ResourceManager& resourceManager) = 0;
		virtual void run(float ts, ResourceManager& resourceManager) = 0;
		virtual void shutdown() = 0;

		virtual ~Game() = default;
	protected:
	};
}
