#pragma once
#include <string>
struct GraphicsSettings
{
	std::string windowTitle = "Rehti engine";
	uint32_t concurrentFrames = 2;
	bool windowCapability = true;
};

int initializeGraphics(const GraphicsSettings& settings);


void eventLoop();

int cleanupGraphics();