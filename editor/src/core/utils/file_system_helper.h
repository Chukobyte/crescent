#pragma once

#include <filesystem>

namespace FileSystemHelper {
std::string GetCurrentDirectory() {
    return std::filesystem::current_path().string();
}
}
