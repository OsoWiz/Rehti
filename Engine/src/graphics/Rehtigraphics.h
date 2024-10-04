#pragma once
#include <string>
struct GraphicsSettings
{
	std::string windowTitle = "Rehti engine";
	uint32_t concurrentFrames = 2;
	bool windowCapability = true; // whether graphical output is desired
	bool dynamicVertexInput = false; // Used when the vertex input should be dynamic and specified at draw time
};

int initializeGraphics(const GraphicsSettings& settings);


void eventLoop();

int cleanupGraphics();