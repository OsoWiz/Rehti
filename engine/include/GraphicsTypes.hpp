#pragma once
#include <RehtiAsset.hpp>

// Structure used to configure the rasterizer state.
struct RasterizationConfig
{
	// Enums.
	// Dynamic means the setting is specified at draw command recording.
	enum class PolygonFillMode
	{
		FILL,
		LINE,
		POINT,
		DYNAMIC
	};

	enum class CullMode
	{
		NONE,
		FRONT,
		BACK,
		FRONT_AND_BACK,
		DYNAMIC
	};

	enum class WindingOrder
	{
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	PolygonFillMode polygonFillMode = PolygonFillMode::LINE;
	CullMode cullMode = CullMode::BACK;
	WindingOrder frontFace = WindingOrder::CLOCKWISE;
};

struct DepthStencilConfig
{
	enum class DepthCompareOp
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_OR_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_OR_EQUAL,
		ALWAYS
	};

	bool depthTestEnable = true;
	DepthCompareOp depthCompareOp = DepthCompareOp::LESS;
	bool stencilTestEnable = false;
	// TODO add stencil operations and masks.
	std::pair<float, float> depthBounds = { 0.0f, 1.0f };
};

// Helper structure for configuring graphics pipelines.
struct GraphicsPipelineConfig
{
	ShaderInterface vertexShader;
	ShaderInterface fragmentShader;

	RasterizationConfig rasterizationConfig;
	DepthStencilConfig depthStencilConfig;
};