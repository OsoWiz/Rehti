enable_testing()

find_package(GTest CONFIG REQUIRED)

set(TEST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/RehtiTests.cpp
)

add_executable(
tests
${TEST_SOURCES}
)

target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main engine)

