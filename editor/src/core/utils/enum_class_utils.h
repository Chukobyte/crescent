#pragma once

#define GENERATE_ENUM_CLASS_OPERATORS(EnumClass)                                                                       \
    inline EnumClass operator|(EnumClass lhs, EnumClass rhs) {                                                         \
        using T = std::underlying_type_t<EnumClass>;                                                                   \
        return static_cast<EnumClass>(static_cast<T>(lhs) | static_cast<T>(rhs));                                      \
    }                                                                                                                  \
                                                                                                                       \
    inline EnumClass& operator|=(EnumClass& lhs, EnumClass rhs) {                                                      \
        lhs = lhs | rhs;                                                                                               \
        return lhs;                                                                                                    \
    }                                                                                                                  \
    inline EnumClass operator&(EnumClass lhs, EnumClass rhs) {                                                         \
        using T = std::underlying_type_t<EnumClass>;                                                                   \
        return static_cast<EnumClass>(static_cast<T>(lhs) & static_cast<T>(rhs));                                      \
    }                                                                                                                  \
                                                                                                                       \
    inline EnumClass& operator&=(EnumClass& lhs, EnumClass rhs) {                                                      \
        lhs = lhs & rhs;                                                                                               \
        return lhs;                                                                                                    \
    }
