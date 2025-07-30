#include "Rehti.hpp"
#include "RehtiGraphics.hpp"

int initialize()
{
	GraphicsSettings settings;
	settings.windowCapability = true;
	settings.windowTitle = "Rehti";
	initializeGraphics(settings);
	return 0;
}


void cleanup()
{
	cleanupGraphics();
}