
project(engine)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Variables
set(REHTI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(REHTI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CORE_SOURCE_DIR ${REHTI_SOURCE_DIR}/core)
set(GRAPHICS_SOURCE_DIR ${REHTI_SOURCE_DIR}/graphics/)
set(PHYSICS_SOURCE_DIR ${REHTI_SOURCE_DIR}/physics/)
set(AUDIO_SOURCE_DIR ${REHTI_SOURCE_DIR}/audio/)
set(INPUT_SOURCE_DIR ${REHTI_SOURCE_DIR}/input/)
set(NETWORKING_SOURCE_DIR ${REHTI_SOURCE_DIR}/networking/)
set(UTIL_SOURCE_DIR ${REHTI_SOURCE_DIR}/util/)
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/)

set(GRAPHICS_SOURCES 
	${GRAPHICS_SOURCE_DIR}/RehtiGraphics.hpp
	${GRAPHICS_SOURCE_DIR}/RehtiGraphics.cpp
	${GRAPHICS_SOURCE_DIR}/VulkanBackend.hpp
	${GRAPHICS_SOURCE_DIR}/VulkanBackend.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsTypes.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsTypes.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsResources.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsResources.cpp
	${GRAPHICS_SOURCE_DIR}/Vertex.hpp
	${GRAPHICS_SOURCE_DIR}/Vertex.cpp
	${GRAPHICS_SOURCE_DIR}/Camera.hpp
	${GRAPHICS_SOURCE_DIR}/Camera.cpp
	${GRAPHICS_SOURCE_DIR}/DescriptorBuilder.hpp
	${GRAPHICS_SOURCE_DIR}/DescriptorBuilder.cpp
	${GRAPHICS_SOURCE_DIR}/ShaderTools.hpp
	${GRAPHICS_SOURCE_DIR}/ShaderTools.cpp
	${GRAPHICS_SOURCE_DIR}/PipelineManager.hpp
	${GRAPHICS_SOURCE_DIR}/PipelineManager.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsAssetCache.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsAssetCache.cpp
	${GRAPHICS_SOURCE_DIR}/EmbeddedShaders.hpp
	${GRAPHICS_SOURCE_DIR}/UIManager.hpp
	${GRAPHICS_SOURCE_DIR}/UIManager.cpp
)

set(PHYSICS_SOURCES 
	${PHYSICS_SOURCE_DIR}/RehtiPhysics.hpp
	${PHYSICS_SOURCE_DIR}/RehtiPhysics.cpp
)
set(AUDIO_SOURCES 
	${AUDIO_SOURCE_DIR}/RehtiAudio.hpp
	${AUDIO_SOURCE_DIR}/RehtiAudio.cpp
)
set(INPUT_SOURCES 
	${INPUT_SOURCE_DIR}/RehtiInput.hpp
	${INPUT_SOURCE_DIR}/RehtiInput.cpp
)
set(NETWORKING_SOURCES 
	${NETWORKING_SOURCE_DIR}/RehtiNetworking.hpp
	${NETWORKING_SOURCE_DIR}/RehtiNetworking.cpp
)

# set(UTIL_SOURCES
# 	${UTIL_SOURCE_DIR}/GlmModule.hpp
# 	${UTIL_SOURCE_DIR}/ShadercModule.hpp
# )


include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${REHTI_INCLUDE_DIR}
	${CORE_SOURCE_DIR}
	${VULKAN_SDK}/include
	${GRAPHICS_SOURCE_DIR}
	${PHYSICS_SOURCE_DIR}
	${AUDIO_SOURCE_DIR}
	${INPUT_SOURCE_DIR}
	${NETWORKING_SOURCE_DIR}
	${SHADERS_DIR}
#	${UTIL_SOURCE_DIR}
)

set(ENGINE_SOURCES 
	${REHTI_INCLUDE_DIR}/Rehti.hpp
	${REHTI_SOURCE_DIR}/Rehti.cpp
	${REHTI_INCLUDE_DIR}/Configuration.hpp
	${CORE_SOURCE_DIR}/Configuration.cpp
	${CORE_SOURCE_DIR}/RehtiAlloc.hpp
	${CORE_SOURCE_DIR}/RehtiAlloc.cpp
	${CORE_SOURCE_DIR}/EntityManager.hpp
	${CORE_SOURCE_DIR}/EntityManager.cpp
	${CORE_SOURCE_DIR}/AssetLoader.hpp
	${CORE_SOURCE_DIR}/AssetLoader.cpp
	${CORE_SOURCE_DIR}/BasicAttributes.hpp
	${CORE_SOURCE_DIR}/BasicAttributes.cpp
	${CORE_SOURCE_DIR}/TaggedPointer.hpp
	${CORE_SOURCE_DIR}/TaggedPointer.cpp
	${CORE_SOURCE_DIR}/EngineSubsystem.hpp
	${CORE_SOURCE_DIR}/EngineSubsystem.cpp
	${GRAPHICS_SOURCES}
	${PHYSICS_SOURCES}
	${AUDIO_SOURCES}
	${INPUT_SOURCES}
	${NETWORKING_SOURCES}
	${UTIL_SOURCES}
	)


add_library(engine ${ENGINE_SOURCES})

# filters
source_group("graphics" FILES ${GRAPHICS_SOURCES})
source_group("physics" FILES ${PHYSICS_SOURCES})
source_group("audio" FILES ${AUDIO_SOURCES})
source_group("input" FILES ${INPUT_SOURCES})
source_group("networking" FILES ${NETWORKING_SOURCES})
source_group("util" FILES ${UTIL_SOURCES})
# source_group("shaders" SOURCES ${SHADER_FILES})

target_link_libraries(engine
	Vulkan::Headers
	Vulkan::Vulkan
	GPUOpen::VulkanMemoryAllocator
	Vulkan::Loader
	SPIRV-Tools-static
	glm::glm
	SDL3::SDL3
	$<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,SDL3_image::SDL3_image,SDL3_image::SDL3_image-static>
	$<IF:$<TARGET_EXISTS:flecs::flecs>,flecs::flecs,flecs::flecs_static>
	jsoncpp_lib 
	glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper
	unofficial::shaderc::shaderc
	imgui::imgui
	)

add_subdirectory(tools)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# cmake deps
add_dependencies(engine GenerateShaders)