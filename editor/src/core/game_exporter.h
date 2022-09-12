#pragma once

#include <string>

namespace GameExporter {
struct ExportProperties {
    std::string gameTitle;
    std::string exportName;
    std::string exportPath;
    std::string projectPath;
    std::string binPath;
    std::string tempPath;
};

void Export(const ExportProperties& props);
} // namespace GameExporter
