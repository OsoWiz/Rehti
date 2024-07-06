
project(Engine)

set(REHTI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(REHTI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(GRAPHICS_SOURCES 
	${REHTI_SOURCE_DIR}/graphics/RehtiGraphics.h
	${REHTI_SOURCE_DIR}/graphics/RehtiGraphics.cpp )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${REHTI_INCLUDE_DIR}
	${VULKAN_SDK}/include
	${REHTI_SOURCE_DIR}/graphics
)

set(ENGINE_SOURCES 
	${REHTI_INCLUDE_DIR}/Rehti.h
	${REHTI_SOURCE_DIR}/Rehti.cpp
	${GRAPHICS_SOURCES}
	)

add_library(engine ${ENGINE_SOURCES})

target_link_libraries(engine
	Vulkan::Headers
	Vulkan::Vulkan
	GPUOpen::VulkanMemoryAllocator
	Vulkan::Loader
	SPIRV-Tools-static
	glm::glm
	SDL2::SDL2
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
	jsoncpp_lib 
	glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper
	unofficial::shaderc::shaderc
	imgui::imgui
	)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
