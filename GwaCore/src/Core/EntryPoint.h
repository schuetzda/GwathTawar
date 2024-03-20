#pragma once

#include "Application.h"

extern std::unique_ptr<gwa::Application> gwa::createApplication();

int main(int argc, char** argv)
{
	auto app = gwa::createApplication();

	app->run();
}