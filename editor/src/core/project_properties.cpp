#include "project_properties.h"

#include "../engine/src/core/game_properties.h"
#include "../engine/src/core/scripting/python/rbe_py.h"
#include "../engine/src/core/utils/logger.h"

ProjectProperties::ProjectProperties(singleton) {
    rbe_game_props_initialize(false);
}

ProjectProperties::~ProjectProperties() {
    rbe_game_props_finalize();
}

void ProjectProperties::LoadPropertiesFromConfig(const char* modulePath) {
    rbe_logger_debug("Loading game properties");
    rbe_game_props_initialize(false);
    RBEGameProperties gameProps = rbe_py_read_config_path(modulePath);
    gameTitle = std::string(gameProps.gameTitle);
    windowWidth = gameProps.windowWidth;
    windowHeight = gameProps.windowHeight;
    targetFPS = gameProps.targetFPS;
    rbe_logger_debug("Loading game properties finished");
}

void ProjectProperties::PrintProperties() const {
    rbe_logger_debug("game_title = %s, window_width = %d, window_height = %d, target_fps = %d",
                     gameTitle.c_str(), windowWidth, windowHeight, targetFPS);
}
