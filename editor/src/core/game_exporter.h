#pragma once

#include <string>
#include <filesystem>

namespace GameExporter {
enum class Platform {
    Undefined,
    Windows,
    Linux,
    MacOS
};

Platform GetPlatformFromString(const std::string& platformString);

struct ExportProperties {
    std::string gameTitle;
    std::filesystem::path exportArchivePath;
    std::filesystem::path projectPath;
    std::filesystem::path tempPath;
    std::string platform;
};

void Export(const ExportProperties& props);
} // namespace GameExporter
