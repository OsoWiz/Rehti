#pragma once
#include <vulkan/vulkan.h>
#include <array>
#include <vector>
#include <optional>
#include "BasicAttributes.hpp"
#include "RehtiAsset.hpp"


struct DrawDetails
{
	uint32_t vertexCount;
	uint32_t indexCount;
	VertexAttributeFlags vertexAttributes;
	size_t stride;
	size_t vertexBufferSize;
	size_t indexBufferSize;
};

struct IndexedDrawable
{
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkDescriptorSet descriptorSet;
	DrawDetails details;
};

struct IndexedDrawableList
{
	std::vector<IndexedDrawable> drawables;
};

struct TriangleFanDrawable
{
	VkBuffer vertexBuffer;
	VkDescriptorSet descriptorSet;
	DrawDetails details;
};

struct QueueDetails
{
	VkQueue queue;
	uint32_t familyIndex;
};