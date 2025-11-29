#pragma once

#include <string>
#include <VulkanBackend.hpp>
#include <EngineSubsystem.hpp>

class Configuration;

class RehtiGraphics : public EngineSubsystem<RehtiGraphics>
{

public:
	static RehtiGraphics* getInstance() { return instance; }

	static int initialize(const Configuration& config);

	static int cleanup();

private:
	inline static RehtiGraphics* instance = nullptr; // Singleton instance

	RehtiGraphics() = default;
	RehtiGraphics(const RehtiGraphics&) = delete;
	RehtiGraphics& operator=(const RehtiGraphics&) = delete;
	static VulkanBackend backendInstance; // Singleton instance of VulkanBackend
};