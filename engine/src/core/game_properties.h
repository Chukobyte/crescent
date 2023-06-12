#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct CREAssetAudioSource {
    char* file_path;
} CREAssetAudioSource;

typedef struct CREAssetTexture {
    char* file_path;
    char* wrap_s;
    char* wrap_t;
    bool applyNearestNeighbor;
} CREAssetTexture;

typedef struct CREAssetFont {
    char* file_path;
    char* uid;
    int size;
    bool applyNearestNeighbor;
} CREAssetFont;

// Only used for having a default font struct
struct CreAssetFontDefault {
    const char* file_path;
    const char* uid;
    int size;
    bool applyNearestNeighbor;
};
static struct CreAssetFontDefault CRE_DEFAULT_FONT_ASSET = {
    .file_path = "assets/fonts/verdana.ttf",
    .uid = "_cre_default",
    .size = 16,
    .applyNearestNeighbor = true
};

#define CRE_INPUT_VALUES_LIMIT 4

typedef struct CREInputAction {
    char* name;
    int deviceId;
    size_t valueCount;
    char* values[CRE_INPUT_VALUES_LIMIT];
} CREInputAction;

#define CRE_PROPERTIES_ASSET_LIMIT 64

typedef struct CREGameProperties {
    char* gameTitle;
    int resolutionWidth;
    int resolutionHeight;
    int windowWidth;
    int windowHeight;
    bool maintainAspectRatio;
    uint32_t audioWaveSampleRate;
    int targetFPS;
    char* initialScenePath;
    bool areCollidersVisible;
    CREAssetAudioSource audioSources[CRE_PROPERTIES_ASSET_LIMIT];
    size_t audioSourceCount;
    CREAssetTexture textures[CRE_PROPERTIES_ASSET_LIMIT];
    size_t textureCount;
    CREAssetFont fonts[CRE_PROPERTIES_ASSET_LIMIT];
    size_t fontCount;
    CREInputAction inputActions[CRE_PROPERTIES_ASSET_LIMIT];
    size_t inputActionCount;
} CREGameProperties;

CREGameProperties* cre_game_props_create();
void cre_game_props_initialize(CREGameProperties* initialProps);
void cre_game_props_finalize();
CREGameProperties* cre_game_props_get();
CREGameProperties* cre_game_props_get_or_default();
void cre_game_props_print();

#ifdef __cplusplus
}
#endif
