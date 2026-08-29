#include "RehtiEngine.hpp"
#include "Logger.hpp"
#include "EventHandlerService.hpp"

#include <flecs.h>
#include <SDL3/SDL_init.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

class RehtiEngine::RehtiImpl : public SDLEventHandler
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
	bool m_active = true;

	bool doesAccept(const SDL_EventType& type) const override;
	void handleEvent(const SDL_Event& event);

	RehtiImpl(const Configuration& configuration);
	~RehtiImpl();
};

std::unique_ptr<RehtiEngine::RehtiImpl> RehtiEngine::instance = nullptr;

RehtiEngine::RehtiImpl::~RehtiImpl()
{
	Logger::info("Implementation destructor called. Shutting down Rehti Engine...");
	EventHandlerService::shutdown();
	for (auto& comp : components)
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

bool RehtiEngine::RehtiImpl::doesAccept(const SDL_EventType& type) const
{
	return  type == SDL_EVENT_QUIT
		|| (type >= SDL_EVENT_WILL_ENTER_BACKGROUND
			&& type <= SDL_EVENT_SYSTEM_THEME_CHANGED);
}

void RehtiEngine::RehtiImpl::handleEvent(const SDL_Event& event)
{
	if (event.type == SDL_EVENT_QUIT)
	{
		Logger::info("Received quit event. Exiting application.");
		exit(0);
	}
	else if (event.type >= SDL_EVENT_WILL_ENTER_BACKGROUND && event.type <= SDL_EVENT_SYSTEM_THEME_CHANGED)
	{
		switch (event.type)
		{
			case SDL_EVENT_WILL_ENTER_BACKGROUND:
				Logger::info("Application will enter background.");
				m_active = false;
				break;
			case SDL_EVENT_DID_ENTER_BACKGROUND:
				Logger::info("Application did enter background.");
				m_active = false;
				break;
			case SDL_EVENT_WILL_ENTER_FOREGROUND:
				Logger::info("Application will enter foreground.");
				m_active = true;
				break;
			case SDL_EVENT_DID_ENTER_FOREGROUND:
				Logger::info("Application did enter foreground.");
				m_active = true;
				break;
			case SDL_EVENT_LOCALE_CHANGED:
				Logger::info("Locale changed.");
				break;
			case SDL_EVENT_SYSTEM_THEME_CHANGED:
				Logger::info("System theme changed.");
				break;
			default:
				Logger::warning("Unhandled application event type: " + std::to_string(event.type));
				break;
		}
	}
}

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
	EventHandlerService::initialize();
	EventHandlerService::getInstance()->registerHandler(instance.get());
	// Initialize default components
	instance->components.push_back({ "Graphics", std::make_unique<RehtiGraphics>(instance->world)});
	instance->components.push_back({ "ResourceManager", std::make_unique<ResourceManager>(instance->world)});
	instance->components.push_back({ "Input", std::make_unique<RehtiInput>(instance->world)});

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
	instance.reset();
}

IEngineSubsystem* RehtiEngine::getSubSystemByName(const std::string& name)
{
	return instance->componentsByName[name];
}

flecs::world& RehtiEngine::getWorld()
{
	return instance->world;
}

void RehtiEngine::progress()
{
	constexpr size_t MAX_EVENTS_PER_FRAME = 2ul << 7ul;
	SDL_Event event;
	EventHandlerService* servicePtr = EventHandlerService::getInstance();
	for (size_t i = 0; i < MAX_EVENTS_PER_FRAME && SDL_PollEvent(&event); ++i)
	{
		if (SDL_EventType(event.type) == SDL_EVENT_QUIT)
		{
			Logger::info("Received quit event. Exiting application.");
			exit(0);
		}
		servicePtr->distributeEvent(event);
	}
	if (instance->m_active)
	{
		instance->world.progress();
	}
}

IEngineSubsystem* RehtiEngine::getSubSystemByType(const std::type_info& type)
{
    auto subSystemIt = std::find_if(instance->components.begin(), instance->components.end(),
		[&type](const RehtiImpl::EngineComponent& comp) {
			return typeid(*comp.ptr.get()) == type;
		});
	return subSystemIt->ptr.get();
}
