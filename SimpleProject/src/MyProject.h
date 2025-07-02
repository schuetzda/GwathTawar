#pragma once
#include <GwathTawar.h>
#include <deque>
#include <ecs/components/RenderObjects.h>
#include <random>
	
class MyProject : public gwa::Game
{
public:
	void init(gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera) override;
	void initRenderGraph(gwa::ntity::Registry& registry, const gwa::Window& window, gwa::renderer::RenderGraphDescription& description,const gwa::QuaternionCamera& camera) override;
	void renderUI(float ts) override;
	void run(float ts, gwa::ntity::Registry& registry, gwa::QuaternionCamera& camera) override;
	void shutdown() override;
private:
	glm::vec4 getRandomVec4(float minX, float maxX,
		float minY, float maxY,
		float minZ, float maxZ) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> distX(minX, maxX);
		std::uniform_real_distribution<float> distY(minY, maxY);
		std::uniform_real_distribution<float> distZ(minZ, maxZ);

		return glm::vec4(
			distX(gen),
			distY(gen),
			distZ(gen),
			1.f
		);
	}
	float ftime100{ 0.f };
	float avgFtime100{ 1.f };
	uint32_t frameCount{ 0 };
	const size_t maxFrames{ 100 };
	uint32_t uboEntity;
	uint32_t lightEntity;
	gwa::ntity::ComponentHandle lightsHandle;

	float angle = 0.0f;  // in radians
	float angularSpeed = 1.0f;  // radians per second
	float radius = 0.1f;

	float lightRadius = 10000.f;
	glm::vec3 lightColor = glm::vec3(1.f);


	uint32_t gltfEntity;
};

