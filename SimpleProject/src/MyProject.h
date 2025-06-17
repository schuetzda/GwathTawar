#pragma once
#include <GwathTawar.h>
#include <deque>
	
class MyProject : public gwa::Game
{
public:
	void init(gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera) override;
	void initRenderGraph(gwa::ntity::Registry& registry, const gwa::Window& window, gwa::renderer::RenderGraphDescription& description,const gwa::QuaternionCamera& camera) override;
	void renderUI(float ts) override;
	void run(float ts, gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera) override;
	void shutdown() override;
private:
	float ftime100{ 0.f };
	float avgFtime100{ 1.f };
	uint32_t frameCount{ 0 };
	const size_t maxFrames{ 100 };
	uint32_t uboEntity;
};

