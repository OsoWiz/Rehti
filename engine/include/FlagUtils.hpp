#pragma once

#define ENABLE_FLAG_BITMASK_OPERATORS(EnumClass)                            \
constexpr EnumClass operator|(EnumClass a, EnumClass b)                     \
{																			\
    using T = std::underlying_type_t<EnumClass>;							\
    return static_cast<EnumClass>(static_cast<T>(a) | static_cast<T>(b));	\
}																			\
constexpr EnumClass operator&(EnumClass a, EnumClass b)                     \
{																			\
    using T = std::underlying_type_t<EnumClass>;							\
    return static_cast<EnumClass>(static_cast<T>(a) & static_cast<T>(b));	\
}																			\
constexpr EnumClass operator<<(EnumClass a, EnumClass b)                    \
{																			\
	using T = std::underlying_type_t<EnumClass>;							\
	return static_cast<EnumClass>(static_cast<T>(a) << static_cast<T>(b));	\
}																			\
constexpr EnumClass& operator|=(EnumClass& a, EnumClass b)					\
{																			\
    return a = a | b;														\
}																			\
constexpr EnumClass& operator&=(EnumClass& a, EnumClass b)					\
{																			\
	return a = a & b;														\
}																			\
constexpr EnumClass& operator<<=(EnumClass& a, EnumClass b)					\
{																			\
	return a = a << b;														\
}																			\
constexpr bool hasFlag(EnumClass value, EnumClass flag)                     \
{                                                                           \
	using T = std::underlying_type_t<EnumClass>;                            \
	return (static_cast<T>(value) & static_cast<T>(flag)) != 0;				\
}																			