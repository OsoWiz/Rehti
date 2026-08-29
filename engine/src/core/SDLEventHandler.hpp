#pragma once
#include <SDL3/SDL_events.h>

class SDLEventHandler
{
public:
	virtual bool doesAccept(const SDL_EventType& type) const = 0;
	virtual void handleEvent(const SDL_Event& event) = 0;
};

