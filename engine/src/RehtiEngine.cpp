#include "RehtiEngine.hpp"
#include "Logger.hpp"

#include <flecs.h>
#include <SDL3/SDL_init.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

class RehtiEngine::RehtiImpl
{
public:
	struct EngineComponent
	{
		std::string name;
		std::unique_ptr<IEngineSubsystem> ptr;
	};
	flecs::world world; // Flecs world for ECS management
	std::unordered_map<std::string, IEngineSubsystem*> componentsByName;
	std::vector<EngineComponent> components;
	Configuration config;
	RehtiImpl(const Configuration& configuration);
};

std::unique_ptr<RehtiEngine::RehtiImpl> RehtiEngine::instance = nullptr;

RehtiEngine::RehtiImpl::RehtiImpl(const Configuration& configuration)
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

int RehtiEngine::initializeRehti(const Configuration& configuration)
{
	if (instance != nullptr)
		return -1; // Already initialized
	Logger::instance().initialize(configuration);
	Logger::info("Initializing Rehti Engine...");
	instance = std::make_unique<RehtiImpl>(configuration);
	SDL_InitSubSystem(SDL_INIT_EVENTS);
	// Initialize default components
	instance->components.push_back({ "Graphics", std::make_unique<RehtiGraphics>(instance->world)});
	instance->components.push_back({ "ResourceManager", std::make_unique<ResourceManager>(instance->world)});

	// Add other components like Audio, Physics, Input, Networking similarly
	for (const auto& comp : instance->components)
	{
		comp.ptr->initialize(configuration);
		instance->componentsByName[comp.name] = comp.ptr.get();
	}
	Logger::info("Engine initialized");

	return 0;
}

int RehtiEngine::initializeRehti()
{
	Configuration defaultConfig(getDefaultSettings());
	return initializeRehti(defaultConfig);
}

void RehtiEngine::cleanupRehti()
{
	Logger::info("Shutting down Rehti Engine...");
	for (auto& comp : instance->components)
	{
		if (comp.ptr && comp.ptr->isInitialized())
		{
			Logger::info("Cleaning up component: " + comp.name);
			comp.ptr->preDestroy();
			comp.ptr.reset();
		}
		else
		{
			Logger::debug("Component " + comp.name + " was not initialized, skipping cleanup.");
		}
	}
	Logger::info("Engine shut down");
	Logger::instance().shutdown();
}

IEngineSubsystem* RehtiEngine::getSubSystemByName(const std::string& name)
{
	return instance->componentsByName[name];
}

flecs::world& RehtiEngine::getWorld()
{
	return instance->world;
}

IEngineSubsystem* RehtiEngine::getSubSystemByType(const std::type_info& type)
{
    auto subSystemIt = std::find_if(instance->components.begin(), instance->components.end(),
		[&type](const RehtiImpl::EngineComponent& comp) {
			return typeid(*comp.ptr.get()) == type;
		});
	return subSystemIt->ptr.get();
}
