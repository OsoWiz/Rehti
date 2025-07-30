#pragma once

#include "GraphicsResources.hpp"
#include <cstdint>

struct CachedModelEntry
{
	Buffer vertexBuffer;
	Buffer indexBuffer;
	uint64_t entryId;
	uint32_t referenceCount;
};

class GraphicsAssetCache
{

public:
	GraphicsAssetCache();
	~GraphicsAssetCache();

private:
	VmaAllocator allocator;
};

