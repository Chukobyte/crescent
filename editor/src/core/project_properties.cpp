#include "project_properties.h"

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
    rbe_logger_debug("Loading game properties");
    nlohmann::json propertyJson = JsonHelper::LoadFile(filePath);
    gameTitle = JsonHelper::Get<std::string>(propertyJson, "name");
    initialNodePath = JsonHelper::Get<std::string>(propertyJson, "initial_node_path");
    windowWidth = JsonHelper::Get<int>(propertyJson, "window_width");
    windowHeight = JsonHelper::Get<int>(propertyJson, "window_height");
    resolutionWidth = JsonHelper::Get<int>(propertyJson, "resolution_width");
    resolutionHeight = JsonHelper::Get<int>(propertyJson, "resolution_height");
    targetFPS = JsonHelper::Get<int>(propertyJson, "target_fps");
    areCollidersVisible = JsonHelper::Get<bool>(propertyJson, "colliders_visible");
    assets.SetAssets(JsonHelper::Get<nlohmann::json>(propertyJson, "assets"));
    inputs.SetInputs(JsonHelper::Get<nlohmann::json>(propertyJson, "inputs"));
    rbe_logger_debug("Loading game properties finished");
}

void ProjectProperties::PrintProperties() const {
    rbe_logger_debug("game_title = %s, window_width = %d, window_height = %d, target_fps = %d",
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
    rbe_logger_error("Couldn't find texture at path '%s'", texturePath.c_str());
    static TextureAsset invalid("invalid", "invalid", "invalid", "invalid", "invalid");
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

// Default project property file
std::string ProjectProperties::GetDefaultProjectPropertyFileContent(const std::string& gameTitle) {
    return "from crescent_api import *\n"
           "\n"
           "configure_game(\n"
           "        game_title=\"" + gameTitle + "\",\n"
           "        window_width=800,\n"
           "        window_height=600,\n"
           "        resolution_width=800,\n"
           "        resolution_height=600,\n"
           "        target_fps=66,\n"
           "        initial_node_path=\"\",\n"
           "        colliders_visible=False,\n"
           ")\n"
           "\n"
           "configure_assets(\n"
           "        audio_sources=[],\n"
           "        textures=[],\n"
           "        fonts=[],\n"
           ")\n"
           "\n"
           "configure_inputs(\n"
           "        input_actions=[]\n"
           ")\n";
}

std::string ProjectProperties::GetPathRelativeToProjectPath(const std::string& path) {
    const std::filesystem::path relativePath = std::filesystem::relative(path, projectPath);
    return relativePath.generic_string();
}
