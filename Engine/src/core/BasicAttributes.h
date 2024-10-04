#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>


// basic attributes
using Position = glm::vec3;
using Orientation = glm::quat;
using Scale = glm::vec3;

struct Pose
{
	Position position;
	Orientation orientation;
	Scale scale;

	static Pose interpolate(Pose first, Pose second, float factor);

	glm::mat4 getTransformationMatrix() const;
};

// physics attributes
using Velocity = glm::vec3;
using AngularVelocity = glm::vec3;
using Acceleration = glm::vec3;

