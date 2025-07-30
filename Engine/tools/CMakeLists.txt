
project(ShaderEmbedding)


set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/engine/resources/shaders/)
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/engine/src/graphics)
set(PYTHON_SCRIPT ${CMAKE_SOURCE_DIR}/engine/tools/ShaderEmbedder.py)
set(EMBEDDED_HEADER ${OUTPUT_DIR}/EmbeddedShaders.hpp)


# Find Python
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(Python3_FOUND)
message(STATUS "Python found: version=${Python3_VERSION} interpreter=${Python3_EXECUTABLE}")
message(STATUS "Combining shaders a to a header file")
# Embed all shaders into a single header file
add_custom_command(
    OUTPUT ${EMBEDDED_HEADER}
    COMMAND Python3::Interpreter ${PYTHON_SCRIPT} ${SHADERS_DIR} ${EMBEDDED_HEADER}
    DEPENDS ${SHADERS_DIR}
    COMMENT "Embedding all shaders into ${EMBEDDED_HEADER}")
else()
message(SEND_ERROR "Python 3 not found! Generating prebaked shader header failed!")

endif()

add_custom_target(GenerateShaders ALL DEPENDS ${EMBEDDED_HEADER})

# Add the Python script to a source group
source_group("scripts" FILES ${PYTHON_SCRIPT})

# Include the Python script in the project
add_custom_target(Scripts SOURCES ${PYTHON_SCRIPT})