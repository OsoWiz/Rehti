
project(engine)

set(GRAPHICS_SOURCES 
	${REHTI_INCLUDE_DIR}/RehtiGraphics.hpp
	${GRAPHICS_SOURCE_DIR}/RehtiGraphics.cpp
	${GRAPHICS_SOURCE_DIR}/Camera.hpp
	${GRAPHICS_SOURCE_DIR}/Camera.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsTypesInternal.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsTypesInternal.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsResources.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsResources.cpp
	${GRAPHICS_SOURCE_DIR}/DescriptorBuilder.hpp
	${GRAPHICS_SOURCE_DIR}/DescriptorBuilder.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsResourceManager.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsResourceManager.cpp
	${GRAPHICS_SOURCE_DIR}/GraphicsUtils.hpp
	${GRAPHICS_SOURCE_DIR}/GraphicsUtils.cpp
	${GRAPHICS_SOURCE_DIR}/ShaderTools.hpp
	${GRAPHICS_SOURCE_DIR}/ShaderTools.cpp
	${GRAPHICS_SOURCE_DIR}/PipelineManager.hpp
	${GRAPHICS_SOURCE_DIR}/PipelineManager.cpp
	${GRAPHICS_SOURCE_DIR}/ShaderObject.hpp
	${GRAPHICS_SOURCE_DIR}/ShaderObject.cpp
	${GRAPHICS_SOURCE_DIR}/UIManager.hpp
	${GRAPHICS_SOURCE_DIR}/UIManager.cpp
)

set(PHYSICS_SOURCES 
	${REHTI_INCLUDE_DIR}/RehtiPhysics.hpp
	${PHYSICS_SOURCE_DIR}/RehtiPhysics.cpp
)
set(AUDIO_SOURCES 
	${REHTI_INCLUDE_DIR}/RehtiAudio.hpp
	${AUDIO_SOURCE_DIR}/RehtiAudio.cpp
)
set(INPUT_SOURCES 
	${REHTI_INCLUDE_DIR}/RehtiInput.hpp
	${INPUT_SOURCE_DIR}/RehtiInput.cpp
)
set(NETWORKING_SOURCES 
	${REHTI_INCLUDE_DIR}/RehtiNetworking.hpp
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
	${REHTI_INCLUDE_DIR}/RehtiEngine.hpp
	${REHTI_SOURCE_DIR}/RehtiEngine.cpp
	${REHTI_INCLUDE_DIR}/RehtiFormat.hpp
	${REHTI_INCLUDE_DIR}/Configuration.hpp
	${CORE_SOURCE_DIR}/Configuration.cpp
	${CORE_SOURCE_DIR}/RehtiAlloc.hpp
	${CORE_SOURCE_DIR}/RehtiAlloc.cpp
	${REHTI_INCLUDE_DIR}/ResourceManager.hpp
	${CORE_SOURCE_DIR}/ResourceManager.cpp
	${CORE_SOURCE_DIR}/AssetLoader.hpp
	${CORE_SOURCE_DIR}/AssetLoader.cpp
	${REHTI_INCLUDE_DIR}/BasicAttributes.hpp
	${CORE_SOURCE_DIR}/BasicAttributes.cpp
	${REHTI_INCLUDE_DIR}/RehtiAsset.hpp
	${CORE_SOURCE_DIR}/RehtiAsset.cpp
	${REHTI_INCLUDE_DIR}/GraphicsTypes.hpp
	${CORE_SOURCE_DIR}/TaggedPointer.hpp
	${CORE_SOURCE_DIR}/TaggedPointer.cpp
	${REHTI_INCLUDE_DIR}/EngineSubsystem.hpp
	${CORE_SOURCE_DIR}/EngineSubsystem.cpp
	${REHTI_INCLUDE_DIR}/FlagUtils.hpp
	${REHTI_INCLUDE_DIR}/Logger.hpp
	${REHTI_INCLUDE_DIR}/RehtiException.hpp
	${CORE_SOURCE_DIR}/RehtiException.cpp
	${CORE_SOURCE_DIR}/Logger.cpp
	${GRAPHICS_SOURCES}
	${PHYSICS_SOURCES}
	${AUDIO_SOURCES}
	${INPUT_SOURCES}
	${NETWORKING_SOURCES}
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
	PRIVATE 
	${REHTI_LIBRARIES}
	)

# add_subdirectory(tools)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(engine PUBLIC RESOURCES_DIR="${REHTI_RESOURCES_DIR}")