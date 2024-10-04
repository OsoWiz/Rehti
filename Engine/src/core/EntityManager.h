#pragma once
#include <flecs.h>
#include <string>

#include "EntityAttributes.h"
#include "GraphicsTypes.h"

class EntityManager
{

public:
	EntityManager();
	~EntityManager();

	void createEntity();

	// void createGraphicsEntity(std::string resourcePath);

private:
	flecs::world world;
	flecs::query<IndexedDrawable> drawablesQuery;
};