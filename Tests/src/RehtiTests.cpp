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
	auto& world = RehtiEngine::getWorld();
	flecs::entity entity = world.entity();
	entity.add<Position>();
	glm::vec3 posValue{ 1.0f, 2.0f, 3.0f };
	Position pos = Position( glm::vec3(1.0f, 2.0f, 3.0f) );
	Position pos2( 1.0f, 2.0f, 3.0f );
	entity.set<Position>(pos);
	const Position* retrievedPos = entity.get<Position>();
	EXPECT_EQ(retrievedPos->value, posValue);
	RehtiEngine::cleanupRehti();
}

	

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}