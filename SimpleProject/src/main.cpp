#include <GwathTawar.h>
#include "MyProject.h"

extern void startApplication(const gwa::AppInfo& info, gwa::Game* game);

int main(int argc, char** argv)
{
	gwa::AppInfo info{ "MySimpleProject",1920, 1080 };
	MyProject project;
	gwa::startApplication(info, &project);
}