#include "BasicAttributes.h"

glm::mat4 Pose::getTransformationMatrix() const
{
	glm::mat4 transformation = glm::mat4(1.0f);

	glm::mat4 scalingMat = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 rotatMat = glm::mat4(orientation);
	glm::mat4 translMat = glm::translate(glm::mat4(1.0f), position);
	transformation = translMat * rotatMat * scalingMat;
	return transformation;
}

Pose Pose::interpolate(Pose first, Pose second, float factor)
{
	Pose interpolatedNode{};
	float normalizedTimeClamped = glm::clamp(factor, 0.0f, 1.0f);
	float inverseWeight = 1.f - normalizedTimeClamped;
	interpolatedNode.position = first.position * inverseWeight + second.position * normalizedTimeClamped;
	interpolatedNode.scale = first.scale * inverseWeight + second.scale * normalizedTimeClamped;
	interpolatedNode.orientation = glm::slerp(first.orientation, second.orientation, normalizedTimeClamped);
	return interpolatedNode;
}