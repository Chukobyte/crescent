#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct RBEAssetAudioSource {
    char* file_path;
} RBEAssetAudioSource;

typedef struct RBEAssetTexture {
    char* file_path;
    char* wrap_s;
    char* wrap_t;
    char* filter_min;
    char* filter_mag;
} RBEAssetTexture;

typedef struct RBEAssetFont {
    char* file_path;
    char* uid;
    int size;
} RBEAssetFont;

#define CRE_INPUT_VALUES_LIMIT 4

typedef struct RBEInputAction {
    char* name;
    int deviceId;
    size_t valueCount;
    char* values[CRE_INPUT_VALUES_LIMIT];
} RBEInputAction;

#define CRE_PROPERTIES_ASSET_LIMIT 24

typedef struct RBEGameProperties {
    char* gameTitle;
    int resolutionWidth;
    int resolutionHeight;
    int windowWidth;
    int windowHeight;
    int targetFPS;
    char* initialScenePath;
    bool areCollidersVisible;
    RBEAssetAudioSource audioSources[CRE_PROPERTIES_ASSET_LIMIT];
    size_t audioSourceCount;
    RBEAssetTexture textures[CRE_PROPERTIES_ASSET_LIMIT];
    size_t textureCount;
    RBEAssetFont fonts[CRE_PROPERTIES_ASSET_LIMIT];
    size_t fontCount;
    RBEInputAction inputActions[CRE_PROPERTIES_ASSET_LIMIT];
    size_t inputActionCount;
} RBEGameProperties;

RBEGameProperties* rbe_game_props_create();
void rbe_game_props_initialize(RBEGameProperties* initialProps);
void rbe_game_props_finalize();
RBEGameProperties* rbe_game_props_get();
void rbe_game_props_print();

#ifdef __cplusplus
}
#endif
