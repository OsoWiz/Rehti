#pragma once
#include "SDLEventHandler.hpp"
#include <SDL3/SDL_events.h>
#include <vector>


class EventHandlerService
{
public:
	// Does this require an initializer of sorts?
	// and also what if work is multithreaded.
	static void initialize();
	static bool isInitialized();
	static EventHandlerService* getInstance();
	static void shutdown();

	void registerHandler(SDLEventHandler* handler);

	void distributeEvent(const SDL_Event& event);

	void clearHandlers();

private:
	EventHandlerService() = default;
	static EventHandlerService* instance;
	std::vector<SDLEventHandler*> handlers;
};

