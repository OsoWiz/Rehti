#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

template <typename Derived, typename ValueType>
struct AttributeBase
{
	ValueType value;

	AttributeBase() = default;
	AttributeBase(ValueType&& value) : value(std::move(value)) {}
	AttributeBase(const ValueType& value) : value(value) {}

	// operators on the same type
	Derived operator+(const Derived& other) const {
		return Derived{ value + other.value };
	}
	Derived operator-(const Derived& other) const {
		return Derived{ value - other.value };
	}
	Derived operator*(const Derived& other) const {
		return Derived{ value * other.value };
	}
	Derived operator/(const Derived& other) const {
		return Derived{ value / other.value };
	}

	Derived& operator+=(const Derived& other) {
		value += other.value;
		return static_cast<Derived&>(*this);
	}
	Derived& operator-=(const Derived& other) {
		value -= other.value;
		return static_cast<Derived&>(*this);
	}

	// operators on value types
	Derived operator+(const ValueType& scalar) const {
		return Derived{ value + scalar };
	}
	Derived operator-(const ValueType& scalar) const {
		return Derived{ value - scalar };
	}
	Derived operator*(const ValueType& scalar) const {
		return Derived{ value * scalar };
	}
	Derived operator/(const ValueType& scalar) const {
		return Derived{ value / scalar };
	}
	Derived& operator=(ValueType&& scalar) {
		value = std::move(scalar);
		return static_cast<Derived&>(*this);
	}
	Derived& operator=(const ValueType& scalar) {
		value = scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator+=(const ValueType& scalar) const {
		this->value += scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator-=(const ValueType& scalar) const {
		this->value -= scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator*=(const ValueType& scalar) {
		value *= scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator/=(const ValueType& scalar) {
		value /= scalar;
		return static_cast<Derived&>(*this);
	}
	// operators on basic types. We assume basic type already has an overload for the scalar type.
	Derived operator+(const float scalar) const {
		return Derived{ value + scalar };
	}
	Derived operator-(const float scalar) const {
		return Derived{ value - scalar };
	}
	Derived operator*(const float scalar) const {
		return Derived{ value * scalar };
	}
	Derived operator/(const float scalar) const {
		return Derived{ value / scalar };
	}
	Derived& operator+=(const float scalar) {
		value += scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator-=(const float scalar) {
		value -= scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator*=(const float scalar) {
		value *= scalar;
		return static_cast<Derived&>(*this);
	}
	Derived& operator/=(const float scalar) {
		value /= scalar;
		return static_cast<Derived&>(*this);
	}
};

template <typename Derived>
struct QuaternionAttributeBase : AttributeBase<Derived, glm::quat>
{
	using AttributeBase<Derived, glm::quat>::operator=;
	using AttributeBase<Derived, glm::quat>::AttributeBase;
	// operators
	glm::mat4 toMat4() const {
		return glm::mat4_cast(this->value);
	}
};

// basic attributes
struct Position : AttributeBase<Position, glm::vec3>
{
	using AttributeBase<Position, glm::vec3>::operator=;
	using AttributeBase<Position, glm::vec3>::AttributeBase;
};


struct Orientation : QuaternionAttributeBase<Orientation>
{
	using QuaternionAttributeBase<Orientation>::operator=;
	using QuaternionAttributeBase<Orientation>::QuaternionAttributeBase;
	// convert to glm::mat4
};

struct Scale : AttributeBase<Scale, glm::vec3>
{
	using AttributeBase<Scale, glm::vec3>::operator=;
	using AttributeBase<Scale, glm::vec3>::AttributeBase;
};

struct Pose
{
	Position position;
	Orientation orientation;
	Scale scale;

	static Pose interpolate(Pose first, Pose second, float factor);

	glm::mat4 getTransformationMatrix() const;
};

// physics attributes
struct Velocity :AttributeBase<Velocity, glm::vec3>
{
	using AttributeBase<Velocity, glm::vec3>::operator=;
	using AttributeBase<Velocity, glm::vec3>::AttributeBase;
};
struct AngularVelocity : AttributeBase<AngularVelocity, glm::vec3>
{
	using AttributeBase<AngularVelocity, glm::vec3>::operator=;
	using AttributeBase<AngularVelocity, glm::vec3>::AttributeBase;
};
struct Acceleration : AttributeBase < Acceleration, glm::vec3>
{
	using AttributeBase<Acceleration, glm::vec3>::operator=;
	using AttributeBase<Acceleration, glm::vec3>::AttributeBase;
};

