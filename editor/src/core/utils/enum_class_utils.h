#pragma once

/*
 * Helper macro to create operators for enum classes.
 * '>>=' is used to check if a flag is contained within another (e.g. (EnumClass::One >>= EnumClass::OneAndTwo) == true)
*/
#define GENERATE_ENUM_CLASS_OPERATORS(EnumClass) \
inline EnumClass operator | (EnumClass lhs, EnumClass rhs) { \
using T = std::underlying_type_t <EnumClass>; \
return static_cast<EnumClass>(static_cast<T>(lhs) | static_cast<T>(rhs)); \
} \
\
inline EnumClass& operator |= (EnumClass& lhs, EnumClass rhs) { \
lhs = lhs | rhs; \
return lhs; \
} \
inline EnumClass operator & (EnumClass lhs, EnumClass rhs) { \
using T = std::underlying_type_t <EnumClass>; \
return static_cast<EnumClass>(static_cast<T>(lhs) & static_cast<T>(rhs)); \
} \
\
inline EnumClass& operator &= (EnumClass& lhs, EnumClass rhs) { \
lhs = lhs & rhs; \
return lhs; \
} \
inline bool operator >>= (EnumClass lhs, EnumClass rhs) { \
return (rhs & lhs) ==  lhs; \
}
