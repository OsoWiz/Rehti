#include "GraphicsTypes.hpp"

#include <vulkan/vulkan.hpp>
#include "BasicAttributes.hpp"


void CharacterData::advanceAnimation(float dt)
{
	// todo perhaps this should fill up a list of transformations. The current bone transformations list is a bit hacky.
	// that way stuff like IK could be just plugged in.
	Animation animation = animationData.animations[animationData.currentAnimationIndex];
	// If no animation is set, do nothing.
	if (animation.animationNodes.empty())
		return;

	animationData.currentTicks += dt * animation.ticksPerSecond;
	double trueAnimationtime = fmod(animationData.currentTicks, animation.totalTicks); // loops over the animation
	animationData.currentTicks = trueAnimationtime;
	size_t animationNodeIndex = 0;
	while (animationNodeIndex < animation.animationNodes.size() - 1 && animation.animationNodes[animationNodeIndex + 1].time < trueAnimationtime)
	{
		animationNodeIndex++;
	}

	AnimationNode firstNode = animation.animationNodes[animationNodeIndex];
	AnimationNode secondNode = animation.animationNodes[(animationNodeIndex + 1) % animation.animationNodes.size()];

	double timeDiff = secondNode.time - firstNode.time;
	if (timeDiff < 0.0) // looping
		timeDiff += animation.totalTicks;

	double factor = (trueAnimationtime - firstNode.time) / timeDiff;

	size_t bonesToUpdate = skeleton.bones.size();
	uint32_t boneIndex = 0; // The root bone is always the first bone in the array.
	while (0 < bonesToUpdate)
	{
		BoneNode bone = skeleton.bones[boneIndex];
		glm::mat4 parentTransformation = glm::mat4(1.0f);
		if (-1 < bone.parent) // we assume parents are always updated before children
			parentTransformation = skeleton.boneTransformations[bone.parent];

		glm::mat4 interPolatedTransformation = Pose::interpolate(firstNode.bones[boneIndex], secondNode.bones[boneIndex], factor).getTransformationMatrix();
		skeleton.boneTransformations[boneIndex] = parentTransformation * interPolatedTransformation;

		boneIndex++;
		bonesToUpdate--;
	}
	// TODO make this more efficient
	for (size_t i = 0; i < skeleton.bones.size(); i++)
	{
		skeleton.boneTransformations[i] = inverseGlobalTransformation * skeleton.boneTransformations[i] * skeleton.bones[i].boneOffset;
	}
}

