#include <GwathTawar.h>
#include <Core/EntryPoint.h>

namespace gwa
{
	class SimpleProject : public Application
	{
	public :
		
		using Application::Application;
	};

	std::unique_ptr<Application> createApplication()
	{
		return std::make_unique<Application>(std::string("Test Project"), 1920, 1080);
	}
}