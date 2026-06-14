# CMakeList.txt : CMake project for Rehti, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.23)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (Rehti)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(THIRD_PARTY_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

# find some libraries
find_package(Boost CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(VulkanLoader CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_IMAGE CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(jsoncpp CONFIG)
find_package(glslang CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log log_setup)

# Variables
set(REHTI_ENGINE_DIR ${CMAKE_SOURCE_DIR}/engine)
set(REHTI_INCLUDE_DIR ${REHTI_ENGINE_DIR}/include)
set(REHTI_SOURCE_DIR ${REHTI_ENGINE_DIR}/src)
set(CORE_SOURCE_DIR ${REHTI_SOURCE_DIR}/core)
set(GRAPHICS_SOURCE_DIR ${REHTI_SOURCE_DIR}/graphics/)
set(PHYSICS_SOURCE_DIR ${REHTI_SOURCE_DIR}/physics/)
set(AUDIO_SOURCE_DIR ${REHTI_SOURCE_DIR}/audio/)
set(INPUT_SOURCE_DIR ${REHTI_SOURCE_DIR}/input/)
set(NETWORKING_SOURCE_DIR ${REHTI_SOURCE_DIR}/networking/)
set(UTIL_SOURCE_DIR ${REHTI_SOURCE_DIR}/util/)
set(RESOURCES_DIR ${REHTI_ENGINE_DIR}/resources/)

# Add source to this project's executable.
add_subdirectory(engine)
add_subdirectory(editor)


if(BUILD_TESTS)
  add_subdirectory(tests)
endif()