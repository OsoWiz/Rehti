#pragma once
#include <cstdint>
#include <type_traits>

static constexpr int TAG_SHIFT = 57;
static constexpr int TAG_BITS = 64 - TAG_SHIFT;

// Type template functionality
template <typename... Ts>
struct Types
{
	static constexpr size_t size = sizeof...(Ts);
};

template <typename T, typename... Ts>
struct IndexOf
{
	static constexpr int value = 0;
	static_assert(!std::is_same_v<T, T>, "Type not present in types!");
};

template <typename T, typename... Ts>
struct IndexOf<T, Types<T, Ts...>>
{
	static constexpr int value = 0;
};

template <typename T, typename U, typename... Ts>
struct IndexOf<T, Types<U, Ts...>>
{
	static constexpr int value = 1 + IndexOf<T, Types<Ts...>>::value;
};

template <typename T, typename... Ts>
struct Contains
{
	static constexpr bool value = false;
};

template <typename T, typename U, typename... Ts>
struct Contains<T, Types<U, Ts...>>
{
	static constexpr bool value = std::is_same<T, U>::value || Contains<T, Types<Ts...>>::value;
};


template <typename... Ts>
class TaggedPointer
{
public:
	using TypeList = Types<Ts...>;


	static constexpr uint64_t TAG_MASK = ((1ull << TAG_BITS) - 1) << TAG_SHIFT; // Mask for the tag bits
	static constexpr uint64_t POINTER_MASK = ~TAG_MASK; // Mask for the pointer bits

	TaggedPointer() = default;

	template <typename T>
	TaggedPointer(T* ptr)
	{
		static_assert(Contains<T, TypeList>::value, "Type not in type list");
		setPointer(reinterpret_cast<uint64_t>(ptr));
		setTag(IndexOf<T, TypeList>::value);
	}

	TaggedPointer(uint64_t value) : value(value) {}

	TaggedPointer& operator=(const TaggedPointer& other)
	{
		if (this != &other)
		{
			value = other.value;
		}
		return *this;
	}

	uint64_t getPointer() const { return value & POINTER_MASK; }
	uint64_t getTag() const { return (value & TAG_MASK) >> TAG_SHIFT; }
	void setPointer(uint64_t ptr) { value = (value & TAG_MASK) | (ptr & POINTER_MASK); }
	void setTag(uint64_t tag) { value = (value & POINTER_MASK) | ((tag << 16) & TAG_MASK); }
	uint64_t getValue() const { return value; }
private:
	uint64_t value;
};

