#include "project_properties.h"

#include "../engine/src/core/game_properties.h"
#include "../engine/src/core/scripting/python/rbe_py.h"
#include "../engine/src/core/utils/logger.h"

void ProjectAssets::SetAssets(RBEGameProperties* gameProperties) {
    for (size_t i = 0; i < gameProperties->textureCount; i++) {
        textures.emplace_back(gameProperties->textures[i]);
    }
    for (size_t i = 0; i < gameProperties->audioSourceCount; i++) {
        audioSources.emplace_back(gameProperties->audioSources[i]);
    }
    for (size_t i = 0; i < gameProperties->fontCount; i++) {
        fonts.emplace_back(gameProperties->fonts[i]);
    }
}

ProjectProperties::ProjectProperties(singleton) {
    rbe_game_props_initialize(false);
}

ProjectProperties::~ProjectProperties() {
    rbe_game_props_finalize();
}

void ProjectProperties::LoadPropertiesFromConfig(const char* modulePath) {
    rbe_logger_debug("Loading game properties");
    rbe_game_props_finalize();
    rbe_game_props_initialize(false);
    RBEGameProperties* gameProps = rbe_py_read_config_path(modulePath);
    gameTitle = std::string(gameProps->gameTitle);
    initialNodePath = std::string(gameProps->initialScenePath);
    windowWidth = gameProps->windowWidth;
    windowHeight = gameProps->windowHeight;
    resolutionWidth = gameProps->resolutionWidth;
    resolutionHeight = gameProps->resolutionHeight;
    targetFPS = gameProps->targetFPS;
    areCollidersVisible = gameProps->areCollidersVisible;
    assets.SetAssets(gameProps);
    rbe_logger_debug("Loading game properties finished");
}

void ProjectProperties::PrintProperties() const {
    rbe_logger_debug("game_title = %s, window_width = %d, window_height = %d, target_fps = %d",
                     gameTitle.c_str(), windowWidth, windowHeight, targetFPS);
}
