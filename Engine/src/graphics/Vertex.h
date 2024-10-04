#pragma once
#include <cstdint>
#include <set>
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>

template <int I>
struct VertexAttribute
{
	glm::vec<I> value;
	VkFormat format;
};

using Position = glm::vec3;
using Normal = glm::vec3;
using Color = glm::vec4;
using TexCoord = glm::vec2;
using Tangent = glm::vec3;
using Bitangent = glm::vec3;
using Joints = glm::uvec4;
using Weights = glm::vec4;

// TEMPLATE SPECIALIZATIONS

template<typename T>
struct AttributeInfo;

template<>
struct AttributeInfo<Position>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
};

// Specialization for Normal
template<>
struct AttributeInfo<Normal>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
};

// Specialization for Color
template<>
struct AttributeInfo<Color>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
};

// Specialization for TexCoord
template<>
struct AttributeInfo<TexCoord>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32_SFLOAT;
};

// Specialization for Tangent
template<>
struct AttributeInfo<Tangent>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
};

// Specialization for Joints
template<>
struct AttributeInfo<Joints>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32B32A32_UINT;
};

// Specialization for Weights
template<>
struct AttributeInfo<Weights>
{
	static constexpr VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
};

typedef enum VertexAttributeEnum : uint16_t
{
	POSITION,
	NORMAL,
	COLOR,
	TEXCOORD,
	TANGENT,
	BITANGENT,
	JOINTS,
	WEIGHTS,
	UNDEFINED
};

VkFormat getFormatFromEnum(VertexAttributeEnum attribute);

std::vector<VkVertexInputAttributeDescription> getVertexAttributes(std::set<VertexAttributeEnum> attributes, uint32_t vertexBinding = 0);

// some predefined vertex types

struct BasicVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

struct BasicCharacterVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::uvec4 joints;
	glm::vec4 weights;
};

struct BasicTexturedVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

/**
 * @brief Templated vertex class
 * @tparam ...Attributes are the attributes used by the vertex
 */
template <typename... Attributes>
struct Vertex
{
	std::tuple<Attributes...> attributes;

	template<typename T>
	T& get()
	{
		return std::get<T>(attributes);
	}

	std::array<attributes.size()> getAttributes(uint32_t vertexBinding = 0)
	{
		std::vector<VkVertexInputAttributeDescription> result;
		uint32_t offset = 0;
		uint32_t location = 0;
		getAttributesImpl(result, offset, location, vertexBinding);
		return result;
	}

	const size_t size = sizeof...(Attributes);

	/*const std::array<VkVertexInputAttributeDescription, sizeof...(Attributes)> getAttributes(uint32_t binding = 0)
	{

	}*/

};