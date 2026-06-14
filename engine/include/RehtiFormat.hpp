#pragma once
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <type_traits>

namespace Rehti
{
	enum class Format : uint64_t
	{
		Undefined = 0,

		Int8,
		UInt8,
		Int16,
		UInt16,
		Int32,
		UInt32,
		Int64,
		UInt64,
		Float32,
		Float64,

		Vec2,
		Vec3,
		Vec4,
		IVec2,
		IVec3,
		IVec4,
		UVec2,
		UVec3,
		UVec4,
		DVec2,
		DVec3,
		DVec4,

		Mat2,
		Mat3,
		Mat4,
		DMat2,
		DMat3,
		DMat4
	};

	constexpr std::size_t formatSize(Format format)
	{
		switch (format)
		{
		case Format::Int8: return sizeof(std::int8_t);
		case Format::UInt8: return sizeof(std::uint8_t);
		case Format::Int16: return sizeof(std::int16_t);
		case Format::UInt16: return sizeof(std::uint16_t);
		case Format::Int32: return sizeof(std::int32_t);
		case Format::UInt32: return sizeof(std::uint32_t);
		case Format::Int64: return sizeof(std::int64_t);
		case Format::UInt64: return sizeof(std::uint64_t);
		case Format::Float32: return sizeof(float);
		case Format::Float64: return sizeof(double);
		case Format::Vec2: return sizeof(glm::vec2);
		case Format::Vec3: return sizeof(glm::vec3);
		case Format::Vec4: return sizeof(glm::vec4);
		case Format::IVec2: return sizeof(glm::ivec2);
		case Format::IVec3: return sizeof(glm::ivec3);
		case Format::IVec4: return sizeof(glm::ivec4);
		case Format::UVec2: return sizeof(glm::uvec2);
		case Format::UVec3: return sizeof(glm::uvec3);
		case Format::UVec4: return sizeof(glm::uvec4);
		case Format::DVec2: return sizeof(glm::dvec2);
		case Format::DVec3: return sizeof(glm::dvec3);
		case Format::DVec4: return sizeof(glm::dvec4);
		case Format::Mat2: return sizeof(glm::mat2);
		case Format::Mat3: return sizeof(glm::mat3);
		case Format::Mat4: return sizeof(glm::mat4);
		case Format::DMat2: return sizeof(glm::dmat2);
		case Format::DMat3: return sizeof(glm::dmat3);
		case Format::DMat4: return sizeof(glm::dmat4);
		case Format::Undefined:
		default:
			return 0;
		}
	}

	constexpr std::size_t formatAlignment(Format format)
	{
		switch (format)
		{
		case Format::Int8: return alignof(std::int8_t);
		case Format::UInt8: return alignof(std::uint8_t);
		case Format::Int16: return alignof(std::int16_t);
		case Format::UInt16: return alignof(std::uint16_t);
		case Format::Int32: return alignof(std::int32_t);
		case Format::UInt32: return alignof(std::uint32_t);
		case Format::Int64: return alignof(std::int64_t);
		case Format::UInt64: return alignof(std::uint64_t);
		case Format::Float32: return alignof(float);
		case Format::Float64: return alignof(double);
		case Format::Vec2: return alignof(glm::vec2);
		case Format::Vec3: return alignof(glm::vec3);
		case Format::Vec4: return alignof(glm::vec4);
		case Format::IVec2: return alignof(glm::ivec2);
		case Format::IVec3: return alignof(glm::ivec3);
		case Format::IVec4: return alignof(glm::ivec4);
		case Format::UVec2: return alignof(glm::uvec2);
		case Format::UVec3: return alignof(glm::uvec3);
		case Format::UVec4: return alignof(glm::uvec4);
		case Format::DVec2: return alignof(glm::dvec2);
		case Format::DVec3: return alignof(glm::dvec3);
		case Format::DVec4: return alignof(glm::dvec4);
		case Format::Mat2: return alignof(glm::mat2);
		case Format::Mat3: return alignof(glm::mat3);
		case Format::Mat4: return alignof(glm::mat4);
		case Format::DMat2: return alignof(glm::dmat2);
		case Format::DMat3: return alignof(glm::dmat3);
		case Format::DMat4: return alignof(glm::dmat4);
		case Format::Undefined:
		default:
			return 0;
		}
	}

	template<Format FormatValue>
	struct GlmType;

	template<> struct GlmType<Format::Int8> { using type = std::int8_t; };
	template<> struct GlmType<Format::UInt8> { using type = std::uint8_t; };
	template<> struct GlmType<Format::Int16> { using type = std::int16_t; };
	template<> struct GlmType<Format::UInt16> { using type = std::uint16_t; };
	template<> struct GlmType<Format::Int32> { using type = std::int32_t; };
	template<> struct GlmType<Format::UInt32> { using type = std::uint32_t; };
	template<> struct GlmType<Format::Int64> { using type = std::int64_t; };
	template<> struct GlmType<Format::UInt64> { using type = std::uint64_t; };
	template<> struct GlmType<Format::Float32> { using type = float; };
	template<> struct GlmType<Format::Float64> { using type = double; };
	template<> struct GlmType<Format::Vec2> { using type = glm::vec2; };
	template<> struct GlmType<Format::Vec3> { using type = glm::vec3; };
	template<> struct GlmType<Format::Vec4> { using type = glm::vec4; };
	template<> struct GlmType<Format::IVec2> { using type = glm::ivec2; };
	template<> struct GlmType<Format::IVec3> { using type = glm::ivec3; };
	template<> struct GlmType<Format::IVec4> { using type = glm::ivec4; };
	template<> struct GlmType<Format::UVec2> { using type = glm::uvec2; };
	template<> struct GlmType<Format::UVec3> { using type = glm::uvec3; };
	template<> struct GlmType<Format::UVec4> { using type = glm::uvec4; };
	template<> struct GlmType<Format::DVec2> { using type = glm::dvec2; };
	template<> struct GlmType<Format::DVec3> { using type = glm::dvec3; };
	template<> struct GlmType<Format::DVec4> { using type = glm::dvec4; };
	template<> struct GlmType<Format::Mat2> { using type = glm::mat2; };
	template<> struct GlmType<Format::Mat3> { using type = glm::mat3; };
	template<> struct GlmType<Format::Mat4> { using type = glm::mat4; };
	template<> struct GlmType<Format::DMat2> { using type = glm::dmat2; };
	template<> struct GlmType<Format::DMat3> { using type = glm::dmat3; };
	template<> struct GlmType<Format::DMat4> { using type = glm::dmat4; };

	template<Format FormatValue>
	using GlmTypeT = typename GlmType<FormatValue>::type;

	template<typename T>
	constexpr Format formatOf()
	{
		if constexpr (std::is_same_v<T, std::int8_t>) return Format::Int8;
		else if constexpr (std::is_same_v<T, std::uint8_t>) return Format::UInt8;
		else if constexpr (std::is_same_v<T, std::int16_t>) return Format::Int16;
		else if constexpr (std::is_same_v<T, std::uint16_t>) return Format::UInt16;
		else if constexpr (std::is_same_v<T, std::int32_t>) return Format::Int32;
		else if constexpr (std::is_same_v<T, std::uint32_t>) return Format::UInt32;
		else if constexpr (std::is_same_v<T, std::int64_t>) return Format::Int64;
		else if constexpr (std::is_same_v<T, std::uint64_t>) return Format::UInt64;
		else if constexpr (std::is_same_v<T, float>) return Format::Float32;
		else if constexpr (std::is_same_v<T, double>) return Format::Float64;
		else if constexpr (std::is_same_v<T, glm::vec2>) return Format::Vec2;
		else if constexpr (std::is_same_v<T, glm::vec3>) return Format::Vec3;
		else if constexpr (std::is_same_v<T, glm::vec4>) return Format::Vec4;
		else if constexpr (std::is_same_v<T, glm::ivec2>) return Format::IVec2;
		else if constexpr (std::is_same_v<T, glm::ivec3>) return Format::IVec3;
		else if constexpr (std::is_same_v<T, glm::ivec4>) return Format::IVec4;
		else if constexpr (std::is_same_v<T, glm::uvec2>) return Format::UVec2;
		else if constexpr (std::is_same_v<T, glm::uvec3>) return Format::UVec3;
		else if constexpr (std::is_same_v<T, glm::uvec4>) return Format::UVec4;
		else if constexpr (std::is_same_v<T, glm::dvec2>) return Format::DVec2;
		else if constexpr (std::is_same_v<T, glm::dvec3>) return Format::DVec3;
		else if constexpr (std::is_same_v<T, glm::dvec4>) return Format::DVec4;
		else if constexpr (std::is_same_v<T, glm::mat2>) return Format::Mat2;
		else if constexpr (std::is_same_v<T, glm::mat3>) return Format::Mat3;
		else if constexpr (std::is_same_v<T, glm::mat4>) return Format::Mat4;
		else if constexpr (std::is_same_v<T, glm::dmat2>) return Format::DMat2;
		else if constexpr (std::is_same_v<T, glm::dmat3>) return Format::DMat3;
		else if constexpr (std::is_same_v<T, glm::dmat4>) return Format::DMat4;
		else return Format::Undefined;
	}
}
