#pragma once
#include <vulkan/vulkan.hpp>
#include "src/core/BasicAttributes.h"

constexpr size_t MAX_BONES = 50;
constexpr size_t MAX_ANIMATIONS = 10;

struct IndexedDrawable
{
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	uint32_t indexCount;
	VkDescriptorSet descriptorSet;
};

struct TriangleFanDrawable
{
	VkBuffer vertexBuffer;
	uint32_t vertexCount;
	VkDescriptorSet descriptorSet;
};


struct AnimationNode
{
	double time;                                 ///< time of this animation node in ticks
	std::array<Pose, MAX_BONES> bones; ///< bone orientations
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

struct CharacterData
{
	Pose characterOrientation;                    ///< orientation of the character
	glm::mat4 inverseGlobalTransformation;                  ///< inverse global transformation of the character
	std::array<glm::mat4, MAX_BONES> boneTransformations{}; ///< bone transformation storage data
	std::vector<BoneNode> bones;
	CharacterAnimationData animationData;
	void advanceAnimation(float dt);
};