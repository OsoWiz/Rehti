#include "RehtiInput.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>

// check if SDL_AddEventWatch makes sense. 
// It ignores the return value of a filter. so seems like it may not be useful here.
// perhaps the setEventfilter is actually what is useful for here.

bool SDLCALL testEventFilter(void* userdata, SDL_Event* event)
{
	// Example filter: Only allow keyboard events
	// TODO there exists SDL_EVENT_KEYBOARD_FIRST and SDL_EVENT_KEYBOARD_LAST, but they are not yet available in version 3.4.12
	if (event->type >= SDL_EVENT_KEY_DOWN && event->type <= SDL_EVENT_SCREEN_KEYBOARD_HIDDEN)
	{
		// type is a keyboard event
		return true; // Allow the event
	}
	else if (event->type >= SDL_EVENT_MOUSE_MOTION && event->type <= SDL_EVENT_MOUSE_REMOVED)
	{
		// type is a mouse event
		// TODO translate the event to a mouse event?
		return true; // Allow the event
	}
	else if (event->type >= SDL_EVENT_GAMEPAD_AXIS_MOTION && event->type <= SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED)
	{
		// type is a gamepad event
		return true; // Allow the event
	}


	return false; // Block other events
}

struct RehtiInput::Backend
{
	// TODO something like this: // InputTranslator translator; <- handles sdl to rehti
	// std:::set<uint64_t> registeredInputDevices; ?
};

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
	return false;
}
