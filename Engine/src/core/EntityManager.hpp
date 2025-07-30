#include <flecs.h>
#include <string>

#include "BasicAttributes.hpp"
#include "GraphicsTypes.hpp"

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