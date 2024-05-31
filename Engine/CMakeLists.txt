
project(Engine)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(ENGINE_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Rehti.cpp
	)

add_library(engine ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
