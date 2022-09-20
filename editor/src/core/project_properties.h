#pragma once

#include <string>
#include <utility>
#include <vector>

#include "../engine/src/core/game_properties.h"

#include "utils/singleton.h"

struct TextureAsset {
    TextureAsset() = default;

    explicit TextureAsset(const RBEAssetTexture& texture)
        : file_path(texture.file_path),
          wrap_s(texture.wrap_s),
          wrap_t(texture.wrap_t),
          filter_min(texture.filter_min),
          filter_mag(texture.filter_mag) {}

    TextureAsset(std::string filePath,
                 std::string wrapS = "clamp_to_border",
                 std::string wrapT = "clamp_to_border",
                 std::string filterMin = "nearest",
                 std::string filterMag = "nearest")
        : file_path(std::move(filePath)),
          wrap_s(std::move(wrapS)),
          wrap_t(std::move(wrapT)),
          filter_min(std::move(filterMin)),
          filter_mag(std::move(filterMag)) {}

    std::string file_path;
    std::string wrap_s;
    std::string wrap_t;
    std::string filter_min;
    std::string filter_mag;
};

struct AudioSourceAsset {
    AudioSourceAsset() = default;

    explicit AudioSourceAsset(const RBEAssetAudioSource& audioSource) :
        file_path(audioSource.file_path) {}

    AudioSourceAsset(std::string filePath)
        : file_path(std::move(filePath)) {}

    std::string file_path;
};

struct FontAsset {
    FontAsset() = default;
    explicit FontAsset(const RBEAssetFont& font) :
        file_path(font.file_path),
        uid(font.uid),
        size(font.size) {}

    std::string file_path;
    std::string uid;
    int size;
};

struct ProjectAssets {
    std::vector<TextureAsset> textures;
    std::vector<AudioSourceAsset> audioSources;
    std::vector<FontAsset> fonts;

    void SetAssets(RBEGameProperties* gameProperties);
};

struct ProjectInputAction {
    std::string name;
    int deviceId = 0;
    std::vector<std::string> values;
};

struct ProjectInputs {
    std::vector<ProjectInputAction> actions;

    void SetInputs(RBEGameProperties* gameProperties);
};

class ProjectProperties : public Singleton<ProjectProperties> {
  public:
    std::string gameTitle;
    std::string initialNodePath;
    int windowWidth;
    int windowHeight;
    int resolutionWidth;
    int resolutionHeight;
    int targetFPS;
    bool areCollidersVisible = false;
    ProjectAssets assets;
    ProjectInputs inputs;
    std::string projectPath;

    ProjectProperties(singleton);
    ~ProjectProperties();

    static std::string GetDefaultProjectPropertyFileContent(const std::string& gameTitle);

    void ResetToDefault();

    void LoadPropertiesFromConfig(const char* modulePath);
    void PrintProperties() const;
    void UpdateTextureAsset(const TextureAsset& textureAsset);
    void UpdateAudioSourceAsset(const AudioSourceAsset& audioSourceAsset);
    TextureAsset& GetTextureAsset(const std::string& texturePath);
    bool HasTextureWithPath(const std::string& path) const;
    bool HasAudioSourceWithPath(const std::string& path) const;
    std::string GetPathRelativeToProjectPath(const std::string& relativePath);
};
