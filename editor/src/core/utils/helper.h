#pragma once

#include <string>

namespace Helper {
inline std::string BoolToString(bool value) {
    return value == true ? "true" : "false";
}

inline std::string BoolToStringCaptital(bool value) {
    return value == true ? "True" : "False";
}
} // namespace
