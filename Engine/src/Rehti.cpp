#include <Rehti.h>
#include "Rehtigraphics.h"

int initialize()
{
	GraphicsSettings settings;
	settings.windowCapability = true;
	settings.windowTitle = "Rehti";
	initializeGraphics(settings);
	cleanupGraphics();
	return 0;
}
