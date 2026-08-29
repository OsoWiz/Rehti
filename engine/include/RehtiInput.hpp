#pragma once
#include <EngineSubsystem.hpp>

enum class EventType
{
	Keyboard,
	Mouse,
	Gamepad
};

struct InputDevice
{
	uint64_t deviceId;
};

enum InputState
{
	Pressed,
	Released,
	Held
};

struct Input
{
	uint64_t inputId;
	InputState state;
};

struct GamepadDirection
{
	float x, y;
};

struct MouseDirection
{
	float x, y;
};

class RehtiInput : public EngineSubsystem<RehtiInput>
{
public:
	RehtiInput(flecs::world& world);
	~RehtiInput();

	int initialize(const Configuration& config) override;
	int preDestroy() override;
	bool isInitialized() const override;

	void setDeviceActive(const InputDevice& device, bool active);


private:
	struct Backend;
	friend struct Backend;
	std::unique_ptr<Backend> backendInstance;
};

