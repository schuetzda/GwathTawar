#pragma once
#include <GwathTawar.h>
class MyProject: public gwa::Game
{
	void init(gwa::ntity::Registry& registry) override;
	void renderUI() override;
	void run(float ts, gwa::ntity::Registry& registry) override;
	void shutdown() override;
};

