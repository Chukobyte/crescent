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
} RBEAssetTexture;

typedef struct RBEAssetFont {
    char* file_path;
    char* uid;
    size_t size;
} RBEAssetFont;

typedef struct RBEInputAction {
    char* name;
    size_t valueCount;
    char* values[4];
} RBEInputAction;

typedef struct RBEGameProperties {
    char* gameTitle;
    int resolutionWidth;
    int resolutionHeight;
    int windowWidth;
    int windowHeight;
    int targetFPS;
    char* initialScenePath;
    RBEAssetAudioSource audioSources[20];
    size_t audioSourceCount;
    RBEAssetTexture textures[20];
    size_t textureCount;
    RBEAssetFont fonts[20];
    size_t fontCount;
    RBEInputAction inputActions[20];
    size_t inputActionCount;
} RBEGameProperties;

void rbe_game_props_initialize(bool loadConfig);
void rbe_game_props_finalize();
RBEGameProperties* rbe_game_props_get();
RBEGameProperties rbe_game_props_read_file(const char* filePath);
void rbe_game_props_print();

#ifdef __cplusplus
}
#endif
