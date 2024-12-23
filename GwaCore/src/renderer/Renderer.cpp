#include "Renderer.h"
#include <glm/glm.hpp>;
#include <glm/ext/matrix_transform.hpp>
namespace gwa {

	std::unique_ptr<RenderAPI> Renderer::s_renderAPI = RenderAPI::Create();
	gwa::Renderer::Renderer() = default;

	void Renderer::init(const Window* window) const
	{
		s_renderAPI->init(window);
	}
	void Renderer::run(const Window* window) 
	{
		float now = (float)window->getTime();
		deltaTime = now - lasttime;
		lasttime = now;

		angle += 10.f * deltaTime;
		if (angle >= 360.f)
		{
			angle = 0.f;
		}

		glm::mat4 firstModel(1.0f);
		glm::mat4 secondModel(1.0f);

		firstModel = glm::translate(firstModel, glm::vec3(0.0f, 0.0f, -2.5f));
		firstModel = glm::rotate(firstModel, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

		secondModel = glm::translate(secondModel, glm::vec3(0.0f, 0.0f, -3.0f));
		secondModel = glm::rotate(secondModel, glm::radians(-angle * 10), glm::vec3(0.0f, 0.0f, 1.0f));

		s_renderAPI->updateModel(0, firstModel);
		s_renderAPI->updateModel(1, secondModel);

		s_renderAPI->draw(window);
	}
	void Renderer::shutdown() const 
	{
		s_renderAPI->shutdown();
	}
}
