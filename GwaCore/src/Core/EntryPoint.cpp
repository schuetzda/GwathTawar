#include "EntryPoint.h"
namespace gwa
{
	void startApplication(const AppInfo& info, Game* game)
	{
		Application app(info, game);
		app.init();
		app.run();
	}
}
