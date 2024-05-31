project(Editor)


set(EDITOR_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/RehtiEditor.cpp
)

add_executable(editor ${EDITOR_SOURCES})

# Link engine
target_link_libraries(editor PRIVATE engine)