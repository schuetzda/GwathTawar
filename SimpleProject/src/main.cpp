#include <GwathTawar.h>
#include "MyProject.h"

int main(int argc, char** argv)
{
	gwa::AppInfo info{ "MySimpleProject",2020, 1080 };
	MyProject project;
	gwa::startApplication(info, &project);
}