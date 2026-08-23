#pragma once
#include <array>
#include <optional>
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
		
		auto& operator[](size_t index) {
			return this->value[index];
		}

		auto operator[](size_t index) const {
			return this->value[index];
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

constexpr size_t MAX_BONES = 50;
constexpr size_t MAX_ANIMATIONS = 10;

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
	size_t getIndexSize() const;
	size_t getStride() const;
	bool isValid() const;
	// calculates tangents and bitangents if missing and can be calculated.
	// returns true if successfully calculated, false otherwise.
	bool calculateTangents();

};

struct GraphicsAsset
{
	Mesh mesh;
	Material material; // TODO asset can have multiple materials.
};

/**
 * @brief Animation node represents a pose of a complete character as a combination of poses of its individual bones at a certain time.
 */
struct AnimationNode
{
	double time;                                 ///< time of this animation node in ticks
	std::array<Pose, MAX_BONES> bones;			 ///< bone orientations
};

/**
 * @brief Immutable animation data. Animations should be stored somewhere and requested when needed to be stored for a character.
 */
struct Animation
{
	double totalTicks;                         ///< total ticks in the animation
	double ticksPerSecond;                     ///< ticks per second
	float duration;                            ///< duration of the animation in seconds
	std::vector<AnimationNode> animationNodes; ///< animation nodes
};

struct CharacterAnimationData
{
	uint32_t currentAnimationIndex;
	double currentTicks;
	std::array<Animation, MAX_ANIMATIONS> animations;
};

struct BoneNode
{
	glm::mat4 boneOffset;           ///< offset matrix of the bone
	int parent;                     ///< index of the parent in bone array.
	std::vector<uint32_t> children; ///< indices of the children in bone array.
};

struct Skeleton
{
	std::vector<glm::mat4> boneTransformations;
	const std::vector<BoneNode> bones;
};

struct CharacterData
{
	Pose characterOrientation;						///< orientation of the character
	glm::mat4 inverseGlobalTransformation;			///< inverse global transformation of the character
	Skeleton skeleton;								///< skeleton of the character
	CharacterAnimationData animationData;			///< animation data of the character
	void advanceAnimation(float dt);				///< advances the current animation of the character
};

struct GraphicsAssetInternal
{
	Mesh mesh;
	VertexAttributeFlags attributes;
	std::vector<Animation> animations;
	std::optional<Skeleton> skeleton;
};


// Todo this is currently exclusively used internally.
// This information could be helpful to the user. So should there be an ability to get a public shadertools instance?
// we need formats etc. in the implementation but they could likely be mapped easily from this interface through other means.
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
	// This deduction is almost always impossible since we have no way of knowing
	// What the attributes are actually used for. We may know there is a vec3 at location 1 but that does not tell anything of use.
	// We could deduce from the name if that is available when reflecting. That would likely be the most correct way.
	VertexAttributeFlags getLikelyVertexAttributes() const;
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

// Pipelineshader is the shader asset and the usage together so it can be used in a pipeline.
struct PipelineShader
{
	ShaderAsset shaderAsset;
	ShaderInterface shaderInterface;
};