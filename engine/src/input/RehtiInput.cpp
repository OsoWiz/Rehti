#include "RehtiInput.hpp"
#include "RehtiException.hpp"
#include "EventHandlerService.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>

struct RehtiInput::Backend : public SDLEventHandler
{
	Backend();
	bool doesAccept(const SDL_EventType& type) const override;
	void handleEvent(const SDL_Event& event) override;
};

RehtiInput::Backend::Backend()
{
	EventHandlerService* instance = EventHandlerService::getInstance();
	if (!instance)
	{
		throw new RehtiException(RehtiError::INITIALIZATION_FAILURE,  "Eventhandler service must be initialized before initializing RehtiInput!");
	}
	instance->registerHandler(this);
}

bool RehtiInput::Backend::doesAccept(const SDL_EventType& type) const
{
	return type >= SDL_EVENT_KEY_DOWN && type <= SDL_EVENT_SCREEN_KEYBOARD_HIDDEN
		|| type >= SDL_EVENT_MOUSE_MOTION && type <= SDL_EVENT_MOUSE_REMOVED
		|| type >= SDL_EVENT_GAMEPAD_AXIS_MOTION && type <= SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED;
}

void RehtiInput::Backend::handleEvent(const SDL_Event& event)
{
	// TODO
}

RehtiInput::RehtiInput(flecs::world& world)
	: EngineSubsystem(world), backendInstance(std::make_unique<Backend>())
{
}

RehtiInput::~RehtiInput()
{}

int RehtiInput::initialize(const Configuration & config)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);
	return 0;
}

int RehtiInput::preDestroy()
{
	return 0;
}

bool RehtiInput::isInitialized() const
{
	return static_cast<bool>(backendInstance);
}
