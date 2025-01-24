#pragma once
#include "Application.h"

namespace gwa
{
	/**
	 * @brief Global function to start the application
	 * @param info Basic Information of the application regarding the application window
	 * @param game Game layer that the application should use.
	 */
	void startApplication(const AppInfo& info, Game* game);
}
