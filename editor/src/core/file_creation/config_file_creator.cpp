#include "config_file_creator.h"

#include "../utils/helper.h"

void ConfigFileCreator::GenerateConfigFile(const char *filePath, ProjectProperties* properties) {
    nlohmann::ordered_json configJson;
    configJson["name"] = properties->gameTitle;
    configJson["window_width"] = properties->windowWidth;
    configJson["window_height"] = properties->windowHeight;
    configJson["resolution_width"] = properties->resolutionWidth;
    configJson["resolution_height"] = properties->resolutionHeight;
    configJson["target_fps"] = properties->targetFPS;
    configJson["initial_node_path"] = properties->initialNodePath;
    configJson["colliders_visible"] = properties->areCollidersVisible;

    // Assets
    nlohmann::ordered_json assetJson;
    // Audio Sources
    nlohmann::ordered_json audioSourceJsonArray = nlohmann::ordered_json::array();
    for (const auto& audioSource : properties->assets.audioSources) {
        nlohmann::ordered_json audioSourceJson;
        audioSourceJson["file_path"] = audioSource.file_path;
        audioSourceJsonArray.emplace_back(audioSourceJson);
    }
    assetJson["audio_sources"] = audioSourceJsonArray;
    // Textures
    nlohmann::ordered_json textureJsonArray = nlohmann::ordered_json::array();
    for (const auto& texture : properties->assets.textures) {
        nlohmann::ordered_json textureJson;
        textureJson["file_path"] = texture.file_path;
        textureJson["wrap_s"] = texture.wrap_s;
        textureJson["wrap_t"] = texture.wrap_t;
        textureJson["filter_min"] = texture.filter_min;
        textureJson["filter_mag"] = texture.filter_mag;
        textureJsonArray.emplace_back(textureJson);
    }
    assetJson["textures"] = textureJsonArray;
    // Fonts
    nlohmann::ordered_json fontJsonArray = nlohmann::ordered_json::array();
    for (const auto& font : properties->assets.fonts) {
        nlohmann::ordered_json fontJson;
        fontJson["file_path"] = font.file_path;
        fontJson["uid"] = font.uid;
        fontJson["size"] = font.size;
        fontJsonArray.emplace_back(fontJson);
    }
    assetJson["fonts"] = fontJsonArray;

    // Inputs
    nlohmann::ordered_json inputsJsonArray = nlohmann::ordered_json::array();
    for (const auto& inputAction : properties->inputs.actions) {
        nlohmann::ordered_json inputActionJson;
        inputActionJson["name"] = inputAction.name;
        inputActionJson["device_id"] = inputAction.deviceId;
        nlohmann::ordered_json valueJsonArray = nlohmann::ordered_json::array();
        for (const std::string& value : inputAction.values) {
            valueJsonArray.emplace_back(value);
        }
        inputActionJson["values"] = valueJsonArray;
    }
    assetJson["inputs"] = inputsJsonArray;

    JsonHelper::SaveFile(filePath, configJson);
}
