enable_testing()

find_package(GTest CONFIG REQUIRED)

set(TEST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/RehtiTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ModelLoaderTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/RehtiGraphicsTests.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${REHTI_INCLUDE_DIR}
	${CORE_SOURCE_DIR}
	${GRAPHICS_SOURCE_DIR}
	${PHYSICS_SOURCE_DIR}
	${AUDIO_SOURCE_DIR}
	${INPUT_SOURCE_DIR}
	${NETWORKING_SOURCE_DIR}
)

add_executable(
tests
${TEST_SOURCES}
)

target_link_libraries(tests 
	PRIVATE 
	GTest::gtest 
	GTest::gtest_main 
	GTest::gmock 
	GTest::gmock_main engine
	${REHTI_LIBRARIES})

target_compile_definitions(tests PRIVATE RESOURCES_DIR="${REHTI_RESOURCES_DIR}")
