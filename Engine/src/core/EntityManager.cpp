#include "EntityManager.hpp"

EntityManager::EntityManager()
{
	this->drawablesQuery = world.query<IndexedDrawable>();
}

EntityManager::~EntityManager()
{
}

void EntityManager::createEntity()
{
	flecs::entity e = world.entity();
	auto id = e.id();
}
