#pragma once


#include <vulkan/vulkan.h>

#include <BasicAttributes.hpp>
// TODO: at some point, there will be situations where e.g. tex coord is not just a vec2, but maybe a vec3 or vec4.
// Refactor at that point. In a sense all that is relevant is whether the attribute sizes match to what the model has.
// And naturally whether shaders in pipeline match their input and output variables.

// TYPEDEFS
namespace VertexAttributes
{
	template <typename VertexAttributeName, typename VertexValueType>
	struct VertexAttributeBase : AttributeBase<VertexAttributeName, VertexValueType>
	{

		using AttributeBase<VertexAttributeName, VertexValueType>::operator=;
		using AttributeBase<VertexAttributeName, VertexValueType>::AttributeBase;
		// convert to VkFormat
		VkFormat getVkFormat() const
		{
			return static_cast<const VertexAttributeName*>(this)->getVkFormatImpl();
		}
		size_t getSize() const
		{
			return sizeof(this->value);
		}
	};

	struct Position : VertexAttributeBase<Position, glm::vec3>
	{
		using VertexAttributeBase<Position, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Position, glm::vec3>::operator=;

		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32_SFLOAT;
		}
	};

	struct Normal : VertexAttributeBase<Normal, glm::vec3>
	{
		using VertexAttributeBase<Normal, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Normal, glm::vec3>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32_SFLOAT;
		}
	};

	struct Color : VertexAttributeBase<Color, glm::vec4>
	{
		using VertexAttributeBase<Color, glm::vec4>::VertexAttributeBase;
		using VertexAttributeBase<Color, glm::vec4>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	};
	struct TexCoord : VertexAttributeBase<TexCoord, glm::vec2>
	{
		using VertexAttributeBase<TexCoord, glm::vec2>::VertexAttributeBase;
		using VertexAttributeBase<TexCoord, glm::vec2>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32_SFLOAT;
		}
	};

	struct Tangent : VertexAttributeBase<Tangent, glm::vec3>
	{
		using VertexAttributeBase<Tangent, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Tangent, glm::vec3>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32_SFLOAT;
		}
	};

	struct Bitangent : VertexAttributeBase<Bitangent, glm::vec3>
	{
		using VertexAttributeBase<Bitangent, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Bitangent, glm::vec3>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32_SFLOAT;
		}
	};
	struct Joints : VertexAttributeBase<Joints, glm::uvec4>
	{
		using VertexAttributeBase<Joints, glm::uvec4>::VertexAttributeBase;
		using VertexAttributeBase<Joints, glm::uvec4>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32A32_UINT;
		}
	};
	struct Weights : VertexAttributeBase<Weights, glm::vec4>
	{
		using VertexAttributeBase<Weights, glm::vec4>::VertexAttributeBase;
		using VertexAttributeBase<Weights, glm::vec4>::operator=;
		VkFormat getVkFormatImpl() const {
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	};

}

enum VertexAttributeEnum : uint16_t
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


enum VertexAttributeFlags : uint16_t
{
	FLAG_NONE = 0,
	FLAG_POSITION = 1 << VertexAttributeEnum::POSITION,
	FLAG_NORMAL = 1 << VertexAttributeEnum::NORMAL,
	FLAG_COLOR = 1 << VertexAttributeEnum::COLOR,
	FLAG_TEXCOORD = 1 << VertexAttributeEnum::TEXCOORD,
	FLAG_TANGENT = 1 << VertexAttributeEnum::TANGENT,
	FLAG_BITANGENT = 1 << VertexAttributeEnum::BITANGENT,
	FLAG_JOINTS = 1 << VertexAttributeEnum::JOINTS,
	FLAG_WEIGHTS = 1 << VertexAttributeEnum::WEIGHTS,
};

// import bitwise operators for VertexAttributeFlags
inline VertexAttributeFlags operator|(VertexAttributeFlags a, VertexAttributeFlags b)
{
	return static_cast<VertexAttributeFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}


struct VertexAttributeInfo
{
	VkFormat format;
	size_t size;
};

VertexAttributeInfo getAttributeInfo(VertexAttributeEnum attribute);

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