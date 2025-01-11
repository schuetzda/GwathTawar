#pragma once
#include <GwathTawar.h>
class MyProject: public gwa::Game
{
	void init(gwa::ResourceManager& resourceManager) override;
	void run(float ts, gwa::ResourceManager& resourceManager) override;
	void shutdown() override;
};

