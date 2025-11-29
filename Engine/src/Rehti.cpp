#include "Rehti.hpp"

#include <flecs.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

#include "Configuration.hpp"
#include "EngineSubsystem.hpp"
#include <RehtiGraphics.hpp>
#include <RehtiAudio.hpp>
#include <RehtiPhysics.hpp>
#include <RehtiInput.hpp>
#include <RehtiNetworking.hpp>

class Rehti::RehtiImpl
{
public:
	struct EngineComponent
	{
		std::string name;
		IEngineSubsystem* ptr;
	};
	flecs::world world; // Flecs world for ECS management
	std::unordered_map<std::string, EngineComponent> componentsByName;
	std::vector<EngineComponent> components;
	Configuration config;
	RehtiImpl(const Configuration& configuration);
};

std::unique_ptr<Rehti::RehtiImpl> Rehti::instance = nullptr;

Rehti::RehtiImpl::RehtiImpl(const Configuration& configuration)
	: config(configuration)
{
}

std::unordered_map<std::string, std::string> getDefaultSettings()
{
	std::unordered_map<std::string, std::string> defaultSettings = {
	   {"window_width", "1280"},
	   {"window_height", "720"},
	   {"fullscreen", "false"},
	   {"audio_enabled", "true"},
	   {"physics_enabled", "true"},
	   {"input_enabled", "true"},
	   {"networking_enabled", "false"}
	};
	return defaultSettings;
}

int Rehti::initializeRehti(const Configuration& configuration)
{
	if (instance != nullptr)
		return -1; // Already initialized
	std::cout << "Initializing Rehti Engine..." << std::endl;
	instance = std::make_unique<RehtiImpl>(configuration);


	RehtiGraphics::initialize(configuration);

	// Initialize default components
	instance->components.push_back({ "Graphics", RehtiGraphics::getInstance() });
	// Add other components like Audio, Physics, Input, Networking similarly
	for (const auto& comp : instance->components)
	{
		instance->componentsByName[comp.name] = comp;
	}
	std::cout << "Engine initialized" << std::endl;

	return 0;
}

int Rehti::initializeRehti()
{
	Configuration defaultConfig(getDefaultSettings());
	return initializeRehti(defaultConfig);
}

void Rehti::cleanupRehti()
{
	std::cout << "Shutting down Rehti Engine..." << std::endl;
	for (auto& comp : instance->components)
	{
		comp.ptr->cleanup();
	}
	std::cout << "Engine shut down" << std::endl;
}
