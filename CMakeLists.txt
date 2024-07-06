# CMakeList.txt : CMake project for Rehti, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.23)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (Rehti)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(GLOBAL PROPERTY CXX_STANDARD 20)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(THIRD_PARTY_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

# find some libraries
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(VulkanLoader CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_IMAGE CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(jsoncpp CONFIG)
find_package(glslang CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# Add source to this project's executable.
add_subdirectory(Engine)
add_subdirectory(Editor)

# TODO: Add tests and install targets if needed.

if(BUILD_TESTS)
  add_subdirectory(Tests)
endif()