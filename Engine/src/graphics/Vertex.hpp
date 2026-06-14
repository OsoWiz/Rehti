#pragma once
#include <vulkan/vulkan.h>
#include <RehtiAsset.hpp>
// TODO: at some point, there will be situations where e.g. tex coord is not just a vec2, but maybe a vec3 or vec4.



// some predefined vertex types

struct BasicVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

struct BasicTexturedVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct BasicCharacterVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::uvec4 joints;
	glm::vec4 weights;
};

struct FullVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec2 texCoord;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::uvec4 joints;
	glm::vec4 weights;
};