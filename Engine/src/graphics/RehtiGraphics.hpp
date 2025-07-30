#pragma once

#include <string>
#include <VulkanBackend.hpp>

// FWD declarations
class VulkanBackend;

// actual decl
struct GraphicsSettings
{
	std::string windowTitle = "Rehti engine";
	uint32_t concurrentFrames = 2;
	bool windowCapability = true; // whether graphical output is desired
	bool dynamicVertexInput = false; // Used when the vertex input should be dynamic and specified at draw time
};

class RehtiGraphics
{

public:
	static int initialize(const GraphicsSettings& graphicsSettings);

	static int cleanup();

private:
	RehtiGraphics() = default;
	RehtiGraphics(const RehtiGraphics&) = delete;
	RehtiGraphics& operator=(const RehtiGraphics&) = delete;
	static VulkanBackend backendInstance; // Singleton instance of VulkanBackend
};