#include <gtest/gtest.h>
#include <Rehti.hpp>

TEST(SampleTest, BasicAssertions) {
	// Expect two strings to be equal.
	EXPECT_STREQ("hello", "hello");
	// Expect two integers to be equal.
	EXPECT_EQ(42, 42);
}

TEST(RehtiTest, Initialization) {
	EXPECT_EQ(Rehti::initializeRehti(), 0);
	Rehti::cleanupRehti();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}