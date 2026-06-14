#pragma once
#include <vector>
#include <BasicAttributes.hpp>
#include <FlagUtils.hpp>
#include <RehtiFormat.hpp>

struct AudioAsset
{

};

// TYPEDEFS
namespace VertexAttributes
{
	template <typename VertexAttributeName, typename VertexValueType>
	struct VertexAttributeBase : AttributeBase<VertexAttributeName, VertexValueType>
	{
		using AttributeBase<VertexAttributeName, VertexValueType>::operator=;
		using AttributeBase<VertexAttributeName, VertexValueType>::AttributeBase;

		static constexpr size_t getSize() {
			return sizeof(VertexValueType);
		}
	};

	struct Position : VertexAttributeBase<Position, glm::vec3>
	{
		using VertexAttributeBase<Position, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Position, glm::vec3>::operator=;
	};

	struct Normal : VertexAttributeBase<Normal, glm::vec3>
	{
		using VertexAttributeBase<Normal, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Normal, glm::vec3>::operator=;
	};

	struct Color : VertexAttributeBase<Color, glm::vec4>
	{
		using VertexAttributeBase<Color, glm::vec4>::VertexAttributeBase;
		using VertexAttributeBase<Color, glm::vec4>::operator=;
	};
	struct TexCoord : VertexAttributeBase<TexCoord, glm::vec2>
	{
		using VertexAttributeBase<TexCoord, glm::vec2>::VertexAttributeBase;
		using VertexAttributeBase<TexCoord, glm::vec2>::operator=;
	};

	struct Tangent : VertexAttributeBase<Tangent, glm::vec3>
	{
		using VertexAttributeBase<Tangent, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Tangent, glm::vec3>::operator=;
	};

	struct Bitangent : VertexAttributeBase<Bitangent, glm::vec3>
	{
		using VertexAttributeBase<Bitangent, glm::vec3>::VertexAttributeBase;
		using VertexAttributeBase<Bitangent, glm::vec3>::operator=;
	};
	struct Joints : VertexAttributeBase<Joints, glm::uvec4>
	{
		using VertexAttributeBase<Joints, glm::uvec4>::VertexAttributeBase;
		using VertexAttributeBase<Joints, glm::uvec4>::operator=;
	};
	struct Weights : VertexAttributeBase<Weights, glm::vec4>
	{
		using VertexAttributeBase<Weights, glm::vec4>::VertexAttributeBase;
		using VertexAttributeBase<Weights, glm::vec4>::operator=;
	};

}

enum class VertexAttributeFlags : uint16_t
{
	NONE = 0,
	POSITION = 1 << 0,
	NORMAL = 1 << 1,
	COLOR = 1 << 2,
	TEXCOORD = 1 << 3,
	TANGENT = 1 << 4,
	BITANGENT = 1 << 5,
	JOINTS = 1 << 6,
	WEIGHTS = 1 << 7,
	UNDEFINED = 1 << 8
};

ENABLE_FLAG_BITMASK_OPERATORS(VertexAttributeFlags);

struct Material
{
	glm::vec4 baseColor;
	glm::vec3 emissiveColor;
	float metallicFactor;
	float roughnessFactor;
};

struct Mesh
{
    std::vector<VertexAttributes::Position> positions;
    std::vector<VertexAttributes::Normal> normals;
    std::vector<VertexAttributes::Color> colors;
    std::vector<VertexAttributes::TexCoord> texCoords;
    std::vector<VertexAttributes::Tangent> tangents;
    std::vector<VertexAttributes::Bitangent> bitangents;
    std::vector<VertexAttributes::Joints> joints;
    std::vector<VertexAttributes::Weights> weights;
    std::vector<uint32_t> indices;
	VertexAttributeFlags getAvailableVertexAttributes() const;
	size_t getSize() const;
	bool isValid() const;
};

struct GraphicsAsset
{
	Mesh mesh;
	Material material; // TODO asset can have multiple materials.
};

struct ShaderInterface
{
	enum class Stage
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		TESSELLATION_CONTROL,
		TESSELLATION_EVALUATION,
		COMPUTE
	};

	struct ShaderInputOutput
	{
		Rehti::Format format;
		uint32_t location;
	};

	struct ResourceBinding
	{
		enum class Type
		{
			SAMPLER,
			COMBINED_IMAGE_SAMPLER,
			SAMPLED_IMAGE,
			STORAGE_IMAGE,
			UNIFORM_BUFFER,
			STORAGE_BUFFER,
			UNIFORM_TEXEL_BUFFER,
			STORAGE_TEXEL_BUFFER,

		};
		uint32_t location, binding;
		Type type;
		size_t size;
	};

    std::vector<ShaderInputOutput> inputs;
    std::vector<ShaderInputOutput> outputs;
    std::vector<ResourceBinding> resources;
	Stage stage;
};

struct ShaderAsset
{
	enum class Format
	{
		SPIRV,
		GLSL,
		HLSL,
		SLANG
	};
	std::vector<uint8_t> bytes;
	Format format;
};