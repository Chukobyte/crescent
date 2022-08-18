#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../engine/src/core/game_properties.h"

#include "utils/singleton.h"

struct ProjectAssets {
    std::vector<RBEAssetTexture> textures;
    std::vector<RBEAssetAudioSource> audioSources;
    std::vector<RBEAssetFont> fonts;

    void SetAssets(RBEGameProperties* gameProperties);
};

struct ProjectInputAction {
    std::string name;
    int deviceId = 0;
    std::vector<std::string> values;
};

struct ProjectInputs {
    std::unordered_map<std::string, ProjectInputAction> actions;

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

    ProjectProperties(singleton);
    ~ProjectProperties();

    void LoadPropertiesFromConfig(const char* modulePath);
    void PrintProperties() const;
};
