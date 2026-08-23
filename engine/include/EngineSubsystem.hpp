#pragma once
#include <flecs.h>
#include <memory>
class Configuration;

// For using generic engine subsystems
struct IEngineSubsystem
{
	virtual ~IEngineSubsystem() = default;
	// gets called after the constructor is called.
	virtual int initialize(const Configuration& config) = 0;
	// This gets called right before the subsystem destructor is called.
	virtual int preDestroy() = 0;
	virtual bool isInitialized() const = 0;
protected:
	friend class RehtiEngine;
};

template <class ImplementingClass>
class EngineSubsystem : public IEngineSubsystem
{
public:

	flecs::world& getWorld() { return world; }

protected:
	EngineSubsystem(flecs::world& ecsWorld) : world(ecsWorld) {}
	flecs::world& world;
};

