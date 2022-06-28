#include "game_properties.h"

#include <stdlib.h>

#include "memory/rbe_mem.h"
#include "scripting/python/rbe_py.h"
#include "utils/logger.h"
#include "utils/rbe_assert.h"

static RBEGameProperties* properties = NULL;

void rbe_game_props_initialize(bool loadConfig) {
    properties                      = RBE_MEM_ALLOCATE(RBEGameProperties);
    properties->gameTitle           = NULL;
    properties->windowWidth         = 800;
    properties->windowHeight        = 600;
    properties->resolutionWidth     = properties->windowWidth;
    properties->resolutionHeight    = properties->windowHeight;
    properties->targetFPS           = 66;
    properties->initialScenePath    = NULL;
    properties->areCollidersVisible = false;
    properties->audioSourceCount    = 0;
    properties->textureCount        = 0;
    properties->fontCount           = 0;
    properties->inputActionCount    = 0;

    if (loadConfig) {
        bool readSuccess = rbe_py_load_project_config();
        RBE_ASSERT_FMT(readSuccess, "Failed to read project configuration!");
    }
}

void rbe_game_props_finalize() {
    RBE_MEM_FREE(properties);
    properties = NULL;
}

RBEGameProperties* rbe_game_props_get() { return properties; }

RBEGameProperties rbe_game_props_read_file(const char* filePath) { return rbe_py_read_config_path(filePath); }

void rbe_game_props_print() {
    if (properties == NULL) {
        rbe_logger_error("No game properties set, not printing!");
        return;
    }
    rbe_logger_debug("game properties:\n    game_title = %s\n    resolution_width = %d\n    resolution_height = %d\n   "
                     " window_width = %d\n    window_height = %d\n    target_fps = %d",
                     properties->gameTitle, properties->resolutionWidth, properties->resolutionHeight,
                     properties->windowWidth, properties->windowHeight, properties->targetFPS);
}
