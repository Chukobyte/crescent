#include "game_properties.h"

#include "../seika/src/utils/logger.h"
#include "../seika/src/memory/rbe_mem.h"
#include "../seika/src/utils/rbe_assert.h"

#include "scripting/python/rbe_py.h"

static RBEGameProperties* properties = NULL;

RBEGameProperties* rbe_game_props_create() {
    RBEGameProperties* props = RBE_MEM_ALLOCATE(RBEGameProperties);
    props->gameTitle = NULL;
    props->windowWidth = 800;
    props->windowHeight = 600;
    props->resolutionWidth = props->windowWidth;
    props->resolutionHeight = props->windowHeight;
    props->targetFPS = 66;
    props->initialScenePath = NULL;
    props->areCollidersVisible = false;
    props->audioSourceCount = 0;
    props->textureCount = 0;
    props->fontCount = 0;
    props->inputActionCount = 0;
    return props;
}

void rbe_game_props_initialize(RBEGameProperties* initialProps) {
    properties = initialProps;
}

void rbe_game_props_finalize() {
    if (properties != NULL) {
        RBE_MEM_FREE(properties);
        properties = NULL;
    }
}

RBEGameProperties* rbe_game_props_get() {
    return properties;
}

RBEGameProperties* cre_game_props_get_or_default() {
    if (properties == NULL) {
        properties = rbe_game_props_create();
    }
    return properties;
}

void rbe_game_props_print() {
    if (properties == NULL) {
        rbe_logger_error("No game properties set, not printing!");
        return;
    }
    rbe_logger_debug("game properties:\n    game_title = %s\n    resolution_width = %d\n    resolution_height = %d\n    window_width = %d\n    window_height = %d\n    target_fps = %d",
                     properties->gameTitle, properties->resolutionWidth, properties->resolutionHeight,properties->windowWidth, properties->windowHeight, properties->targetFPS);
}
