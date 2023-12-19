#pragma once

#include <string>

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
    std::string exportArchivePath;
    std::string projectPath;
    std::string binPath;
    std::string tempPath;
    Platform platform;
};

void Export(const ExportProperties& props);
} // namespace GameExporter
