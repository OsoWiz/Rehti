#pragma once

#include <vulkan/vulkan.h>
#include <array>
#include <vector>
#include "BasicAttributes.hpp"

constexpr size_t MAX_BONES = 50;
constexpr size_t MAX_ANIMATIONS = 10; // Redo with component system?

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