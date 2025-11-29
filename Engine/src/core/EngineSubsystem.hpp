#pragma once

class Configuration;

struct IEngineSubsystem
{
	virtual ~IEngineSubsystem() = default;
	virtual int initialize(const Configuration& config) = 0;
	virtual int cleanup() = 0;
};


template <class ImplementingClass>
class EngineSubsystem : public IEngineSubsystem
{
public:
	static int initialize(const Configuration& config)
	{
		return ImplementingClass::initialize(config);
	}

	static int cleanup()
	{
		return ImplementingClass::cleanup();
	}

};

