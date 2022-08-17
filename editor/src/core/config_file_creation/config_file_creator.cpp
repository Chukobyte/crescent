#include "config_file_creator.h"
#include "../utils/helper.h"
#include "../utils/file_system_helper.h"

void ConfigFileCreator::GenerateConfigFile(const char *filePath, ProjectProperties* properties) {
    std::string fileContents = "from crescent_api import *\n";
    // Configure Game
    fileContents += "\n";
    fileContents += "configure_game(\n";
    fileContents += "   game_title=\"" + properties->gameTitle + "\",\n";
    fileContents += "   window_width=" + std::to_string(properties->windowWidth) + ",\n";
    fileContents += "   window_height=" + std::to_string(properties->windowHeight) + ",\n";
    fileContents += "   resolution_width=" + std::to_string(properties->resolutionWidth) + ",\n";
    fileContents += "   resolution_height=" + std::to_string(properties->resolutionHeight) + ",\n";
    fileContents += "   target_fps=" + std::to_string(properties->targetFPS) + ",\n";
    fileContents += "   initial_node_path=\"" + properties->initialNodePath + "\",\n";
    fileContents += "   colliders_visible=" + Helper::BoolToStringCaptital(properties->areCollidersVisible) + ",\n";
    fileContents += ")\n";
    fileContents += "\n";

    // Configure Assets
    fileContents += "configure_assets(\n";
    // Audio Sources
    fileContents += "   audio_sources=[\n";
    for (const auto& audioSource : properties->assets.audioSources) {
        fileContents += "       AudioSource(file_path=\"" + std::string(audioSource.file_path) + "\"),\n";
    }
    fileContents += "   ],\n";
    // Textures
    fileContents += "   textures=[\n";
    for (const auto& texture : properties->assets.textures) {
        fileContents += "       Texture(\n";
        fileContents += "           file_path=\"" + std::string(texture.file_path) + "\",\n";
        fileContents += "           wrap_s=\"" + std::string(texture.wrap_s) + "\",\n";
        fileContents += "           wrap_t=\"" + std::string(texture.wrap_t) + "\",\n";
        fileContents += "           filter_min=\"" + std::string(texture.filter_min) + "\",\n";
        fileContents += "           filter_mag=\"" + std::string(texture.filter_mag) + "\",\n";
        fileContents += "       ),\n";
    }
    fileContents += "   ],\n";
    // Fonts
    fileContents += "   fonts=[\n";
    for (const auto& font : properties->assets.fonts) {
        fileContents += "       Font(\n";
        fileContents += "           file_path=\"" + std::string(font.file_path) + "\",\n";
        fileContents += "           uid=\"" + std::string(font.uid) + "\",\n";
        fileContents += "           size=" + std::to_string(font.size) + ",\n";
        fileContents += "       ),\n";
    }
    fileContents += "   ],\n";
    // Closing parentheses for configure assets
    fileContents += ")\n";

    FileSystemHelper::WriteFile(filePath, fileContents);
}
