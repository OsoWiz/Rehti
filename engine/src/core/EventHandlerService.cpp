#include "EventHandlerService.hpp"

EventHandlerService* EventHandlerService::instance = nullptr;

bool EventHandlerService::isInitialized()
{
	return instance != nullptr;
}

EventHandlerService* EventHandlerService::getInstance()
{
	return instance;
}

void EventHandlerService::shutdown()
{
	if (instance)
	{
		instance->clearHandlers();
		delete instance;
		instance = nullptr;
	}
}

void EventHandlerService::initialize()
{
	if (!instance)
	{
		instance = new EventHandlerService();
	}
}

void EventHandlerService::registerHandler(SDLEventHandler* handler)
{
	handlers.push_back(handler);
}

void EventHandlerService::distributeEvent(const SDL_Event& event)
{
	// Handle the event here
	for (auto handler : handlers)
	{
		if (handler && handler->doesAccept(SDL_EventType(event.type)))
		{
			handler->handleEvent(event);
		}
	}
}

void EventHandlerService::clearHandlers()
{
	handlers.clear();
}
