#pragma once
#include <vma/vk_mem_alloc.h>

#include "GraphicsResources.h"

struct CachedModelEntry
{
	RehtiGraphics::Buffer vertexBuffer;
	RehtiGraphics::Buffer indexBuffer;
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

