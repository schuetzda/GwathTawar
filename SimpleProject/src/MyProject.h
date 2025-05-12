#pragma once
#include <GwathTawar.h>
#include <deque>
class MyProject: public gwa::Game
{
public:
	void init(gwa::ntity::Registry& registry) override;
	void renderUI(float ts) override;
	void run(float ts, gwa::ntity::Registry& registry) override;
	void shutdown() override;
private:
	float ftime100{ 0.f };
	float avgFtime100{ 1.f };
	uint32_t frameCount{ 0 };
	const size_t maxFrames{ 100 };
};

