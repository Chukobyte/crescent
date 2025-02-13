#include "project_properties.h"

#include <seika/logger.h>
#include <seika/audio/audio.h>

#include "../engine/src/core/core_info.h"

#include "asset_browser.h"

//--- Project Assets ---//
void ProjectAssets::SetAssets(const nlohmann::json &assetsJson) {
    if (JsonHelper::HasKey(assetsJson, "textures")) {
        for (auto& textureJson : JsonHelper::Get<nlohmann::json>(assetsJson, "textures")) {
            textures.emplace_back(TextureAsset(textureJson));
        }
    }
    if (JsonHelper::HasKey(assetsJson, "audio_sources")) {
        for (auto& audioSourceJson : JsonHelper::Get<nlohmann::json>(assetsJson, "audio_sources")) {
            audioSources.emplace_back(AudioSourceAsset(audioSourceJson));
        }
    }
    if (JsonHelper::HasKey(assetsJson, "fonts")) {
        for (auto& fontJson : JsonHelper::Get<nlohmann::json>(assetsJson, "fonts")) {
            fonts.emplace_back(FontAsset(fontJson));
        }
    }
}

//--- Project Inputs ---//
void ProjectInputs::SetInputs(const nlohmann::json &inputsJsonArray) {
    for (auto& inputJson : inputsJsonArray) {
        ProjectInputAction inputAction = {
            .name = JsonHelper::Get<std::string>(inputJson, "name"),
            .deviceId = JsonHelper::GetDefault<int>(inputJson, "device_id", 0)
        };
        for (const std::string& value : JsonHelper::Get<nlohmann::json>(inputJson, "values")) {
            inputAction.values.emplace_back(value);
        }
        actions.emplace_back(inputAction);
    }
}

//--- Project Properties ---//
namespace {
void CreateDefaultAssetIfNonExisting(const FileNode& dirNode) {
    static ProjectProperties* projectProperties = ProjectProperties::Get();
    // Create default asset if it doesn't exist
    for (const auto& fileNode : dirNode.files) {
        const std::string fileName = fileNode.path.filename().string();
        if (fileName.ends_with(".png") && !projectProperties->HasTextureWithPath(fileNode.GetRelativePath())) {
            projectProperties->assets.textures.emplace_back(TextureAsset(fileNode.GetRelativePath()));
        } else if (fileName.ends_with(".wav") && !projectProperties->HasAudioSourceWithPath(fileNode.GetRelativePath())) {
            projectProperties->assets.audioSources.emplace_back(AudioSourceAsset(fileNode.GetRelativePath()));
        }
    }

    // Recurse on all dirs
    for (const auto& childDirNode : dirNode.directories) {
        CreateDefaultAssetIfNonExisting(childDirNode);
    }
}
} // namespace

ProjectProperties::ProjectProperties(singleton) {
    // Register project properties with asset browser refresh cache
    AssetBrowser* assetBrowser = AssetBrowser::Get();
    assetBrowser->RegisterRefreshCallback([this](const FileNode& rootNode) {
        CreateDefaultAssetIfNonExisting(rootNode);
    });
}

void ProjectProperties::ResetToDefault() {
    gameTitle.clear();
    initialNodePath.clear();
    windowWidth = 800;
    windowHeight = 600;
    resolutionWidth = 800;
    resolutionHeight = 600;
    targetFPS = 66;
    areCollidersVisible = false;
    assets.textures.clear();
    assets.audioSources.clear();
    assets.fonts.clear();
    inputs.actions.clear();
}

void ProjectProperties::LoadPropertiesFromConfig(const char* filePath) {
    ska_logger_debug("Loading game properties");
    nlohmann::json propertyJson = JsonHelper::LoadFile(filePath);
    gameTitle = JsonHelper::Get<std::string>(propertyJson, "name");
    initialNodePath = JsonHelper::Get<std::string>(propertyJson, "initial_node_path");
    windowWidth = JsonHelper::Get<int>(propertyJson, "window_width");
    windowHeight = JsonHelper::Get<int>(propertyJson, "window_height");
    resolutionWidth = JsonHelper::Get<int>(propertyJson, "resolution_width");
    resolutionHeight = JsonHelper::Get<int>(propertyJson, "resolution_height");
    maintainAspectRatio = JsonHelper::GetDefault<bool>(propertyJson, "maintain_aspect_ratio", false);
    targetFPS = JsonHelper::Get<int>(propertyJson, "target_fps");
    areCollidersVisible = JsonHelper::Get<bool>(propertyJson, "colliders_visible");
    version = JsonHelper::GetDefault<std::string>(propertyJson, "version", "0.0.1");
    if (JsonHelper::HasKey(propertyJson, "window_background_color")) {
        const auto windowBackgroundColorJson = JsonHelper::Get<nlohmann::json>(propertyJson, "window_background_color");
        const int r = JsonHelper::Get<int>(windowBackgroundColorJson, "r");
        const int g = JsonHelper::Get<int>(windowBackgroundColorJson, "g");
        const int b = JsonHelper::Get<int>(windowBackgroundColorJson, "b");
        const int a = JsonHelper::Get<int>(windowBackgroundColorJson, "a");
        windowBackgroundColor = SkaColor{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
    }
    assets.SetAssets(JsonHelper::Get<nlohmann::json>(propertyJson, "assets"));
    inputs.SetInputs(JsonHelper::Get<nlohmann::json>(propertyJson, "inputs"));
    ska_logger_debug("Loading game properties finished");
}

void ProjectProperties::PrintProperties() const {
    ska_logger_debug("game_title = %s, window_width = %d, window_height = %d, target_fps = %d",
                    gameTitle.c_str(), windowWidth, windowHeight, targetFPS);
}

void ProjectProperties::UpdateTextureAsset(const TextureAsset &textureAsset) {
    for (auto& asset : assets.textures) {
        if (asset.file_path == textureAsset.file_path) {
            asset = textureAsset;
            break;
        }
    }
}

void ProjectProperties::UpdateAudioSourceAsset(const AudioSourceAsset &audioSourceAsset) {
    for (auto& asset : assets.audioSources) {
        if (asset.file_path == audioSourceAsset.file_path) {
            asset = audioSourceAsset;
            break;
        }
    }
}

TextureAsset& ProjectProperties::GetTextureAsset(const std::string &texturePath) {
    for (auto& asset : assets.textures) {
        if (texturePath == asset.file_path) {
            return asset;
        }
    }
    ska_logger_error("Couldn't find texture at path '%s'", texturePath.c_str());
    static TextureAsset invalid("invalid", "invalid", "invalid", true);
    return invalid;
}

bool ProjectProperties::HasTextureWithPath(const std::string &path) const {
    for (const auto& texture : assets.textures) {
        if (texture.file_path == path) {
            return true;
        }
    }
    return false;
}

bool ProjectProperties::HasAudioSourceWithPath(const std::string &path) const {
    for (const auto& audioSource : assets.audioSources) {
        if (audioSource.file_path == path) {
            return true;
        }
    }
    return false;
}

nlohmann::ordered_json ProjectProperties::ToJson() const {
    nlohmann::ordered_json configJson;
    configJson["name"] = gameTitle;
    configJson["window_width"] = windowWidth;
    configJson["window_height"] = windowHeight;
    configJson["resolution_width"] = resolutionWidth;
    configJson["resolution_height"] = resolutionHeight;
    configJson["maintain_aspect_ratio"] = maintainAspectRatio;
    configJson["target_fps"] = targetFPS;
    configJson["initial_node_path"] = initialNodePath;
    configJson["colliders_visible"] = areCollidersVisible;
    configJson["vsync_enabled"] = vsyncEnabled;
    configJson["window_background_color"] = JsonHelper::ColorToJson(windowBackgroundColor);
    configJson["version"] = version;

    // Assets
    nlohmann::ordered_json assetJson;
    // Audio Sources
    nlohmann::ordered_json audioSourceJsonArray = nlohmann::ordered_json::array();
    for (const auto& audioSource : assets.audioSources) {
        nlohmann::ordered_json audioSourceJson;
        audioSourceJson["file_path"] = audioSource.file_path;
        audioSourceJsonArray.emplace_back(audioSourceJson);
    }
    assetJson["audio_sources"] = audioSourceJsonArray;
    // Textures
    nlohmann::ordered_json textureJsonArray = nlohmann::ordered_json::array();
    for (const auto& texture : assets.textures) {
        nlohmann::ordered_json textureJson;
        textureJson["file_path"] = texture.file_path;
        textureJson["wrap_s"] = texture.wrap_s;
        textureJson["wrap_t"] = texture.wrap_t;
        textureJson["nearest_neighbor"] = texture.nearestNeighbor;
        textureJsonArray.emplace_back(textureJson);
    }
    assetJson["textures"] = textureJsonArray;
    // Fonts
    nlohmann::ordered_json fontJsonArray = nlohmann::ordered_json::array();
    for (const auto& font : assets.fonts) {
        nlohmann::ordered_json fontJson;
        fontJson["file_path"] = font.file_path;
        fontJson["uid"] = font.uid;
        fontJson["size"] = font.size;
        fontJson["nearest_neighbor"] = font.applyNearestNeighbor;
        fontJsonArray.emplace_back(fontJson);
    }
    assetJson["fonts"] = fontJsonArray;

    configJson["assets"] = assetJson;

    // Inputs
    nlohmann::ordered_json inputsJsonArray = nlohmann::ordered_json::array();
    for (const auto& inputAction : inputs.actions) {
        nlohmann::ordered_json inputActionJson;
        inputActionJson["name"] = inputAction.name;
        if (inputAction.deviceId != 0) {
            inputActionJson["device_id"] = inputAction.deviceId;
        }
        nlohmann::ordered_json valueJsonArray = nlohmann::ordered_json::array();
        for (const std::string& value : inputAction.values) {
            valueJsonArray.emplace_back(value);
        }
        inputActionJson["values"] = valueJsonArray;
        inputsJsonArray.emplace_back(inputActionJson);
    }
    configJson["inputs"] = inputsJsonArray;

    // Adding in crescent core version to easily see crescent version without running executable
    configJson["crescent_core_version"] = CRE_CORE_VERSION;

    return configJson;
}

std::string ProjectProperties::GetPathRelativeToProjectPath(const std::string& path) {
    const std::filesystem::path relativePath = std::filesystem::relative(path, projectPath);
    return relativePath.generic_string();
}

SkaSize2D ProjectProperties::GetResolutionSize() const {
    return {
        .w = static_cast<f32>(resolutionWidth),
        .h = static_cast<f32>(resolutionHeight)
    };
}
