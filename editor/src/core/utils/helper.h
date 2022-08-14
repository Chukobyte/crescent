#pragma once

#include <string>

namespace Helper {
inline std::string BoolToString(bool value) {
    return value == true ? "true" : "false";
}
} // namespace
