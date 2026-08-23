#pragma once
#include <string>
#include <memory>
#include <EngineSubsystem.hpp>
#include "FlagUtils.hpp"
#include <GraphicsTypes.hpp>

class Configuration;

struct PipelineHandle
{
	uint64_t id;
};

struct GraphicsObjectHandle
{
	uint64_t id;
};

struct AttachedToPipeline
{};

namespace RehtiGraphicsUtil {

	enum class Flags : uint16_t
	{
		NONE,
		WINDOWING = 1 << 0,
		DYNAMIC_RENDERING = 1 << 1, // use dynamic rendering instead of render passes
		DYNAMIC_VERTEX_INPUT = 1 << 2, // use dynamic vertex input instead of pipeline vertex input state
	};
	ENABLE_FLAG_BITMASK_OPERATORS(Flags);
}

class RehtiGraphics : public EngineSubsystem<RehtiGraphics>
{
public:
	// decl
	struct Settings
	{
		std::string windowTitle = "Rehti Engine";
		uint32_t concurrentFrames = 2;
		RehtiGraphicsUtil::Flags flags = RehtiGraphicsUtil::Flags::WINDOWING | RehtiGraphicsUtil::Flags::DYNAMIC_RENDERING;
	};

	RehtiGraphics(flecs::world& world);
	~RehtiGraphics();

	int initialize(const Configuration& config) override;

	int preDestroy() override;

	bool isInitialized() const override;

	// Draws a frame as configured.
	void drawFrame() const;

	PipelineHandle createGraphicsPipeline(const GraphicsPipelineConfig& pipelineConfig);
	GraphicsObjectHandle createGraphicsObject(const Mesh& mesh);

	bool attachGraphicsObjectToPipeline(PipelineHandle pipelineHandle, GraphicsObjectHandle gfxObjectHandle);

private:
	RehtiGraphics(const RehtiGraphics&) = delete;
	RehtiGraphics& operator=(const RehtiGraphics&) = delete;
	struct Backend;
	friend struct Backend;
	std::unique_ptr<Backend> backendInstance;
};