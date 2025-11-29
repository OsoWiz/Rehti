project(Editor)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EDITOR_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/RehtiEditor.cpp
)

add_executable(editor ${EDITOR_SOURCES})

# Link engine
target_link_libraries(editor PRIVATE engine)