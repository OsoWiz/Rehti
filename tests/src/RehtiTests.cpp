#include <gtest/gtest.h>
#include <BasicAttributes.hpp>
#include <RehtiEngine.hpp>

TEST(SampleTest, BasicAssertions) {
	// Expect two strings to be equal.
	EXPECT_STREQ("hello", "hello");
	// Expect two integers to be equal.
	EXPECT_EQ(42, 42);
}

TEST(RehtiTest, Initialization) {
	EXPECT_EQ(RehtiEngine::initializeRehti(), 0);
	EXPECT_EQ(RehtiEngine::getSubSystem<RehtiGraphics>().isInitialized(), true);
	RehtiEngine::cleanupRehti();
}

TEST(RehtiTest, useECSTest) {
	EXPECT_EQ(RehtiEngine::initializeRehti(), 0);

	struct Relation { int type = 1; };
	struct GData { int value = -1; std::string text = "No comment"; };
	struct NewComponent { int value; };
	std::cout << "Testing ecs " << std::endl;
	auto& world = RehtiEngine::getWorld();
	flecs::entity group = world.entity();
	flecs::entity a = world.entity();
	flecs::entity b = world.entity();
	flecs::entity relation = world.entity();
	group.set<GData>({ 42, "Hello World" });
	a.add<Relation>(group);
	b.add<Relation>(group);

	std::cout << "Entity id: " << group.id() << std::endl;
	std::cout << "a id: " << a.id() << std::endl;
	std::cout << "b id: " << b.id() << std::endl;

	a.set<NewComponent>({ 1 });
	b.set<NewComponent>({ 2 });

	NewComponent acomp = a.get<NewComponent>();
	const NewComponent& bcomp = b.get<NewComponent>();

	for (const auto& comp : { acomp, bcomp })
	{
		std::cout << "Component value: " << comp.value << std::endl;
	}

	auto q = world.query<NewComponent>();
	q.each([](flecs::entity e, NewComponent& comp)
		{
			std::cout << "Entity " << e.name() << " has NewComponent with value: " << comp.value << std::endl;
		});

	std::cout << "Creating group query" << std::endl;
	flecs::query<NewComponent> grouped = world.query_builder<NewComponent>().group_by<Relation>().build();
	std::cout << "Grouped query created." << std::endl;
	size_t looped = 0;
	for (auto group : grouped.groups())
	{
		flecs::entity group_entity = world.entity(group.first);

		// Access group data
		const GData& group_data = group_entity.get<GData>();
		std::cout << "Group: " << group_data.text
			<< " (Level: " << group_data.value << ")" << std::endl;

		// Iterate members in this group
		// this works I guess
		grouped.set_group(group.first).each([&](flecs::entity e, NewComponent& comp)
			{
				std::cout << "  Entity: " << e.id() << ", NewComponent value: " << comp.value << std::endl;
			});
		
		
		
		looped++;
	}
	std::cout << "Done with ecs test. Looped " << looped << " times." << std::endl;
	RehtiEngine::cleanupRehti();
}

	

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}