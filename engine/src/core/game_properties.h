#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct RBEAssetAudioSource {
    const char* file_path;
}RBEAssetAudioSource;

typedef struct RBEAssetTexture {
    const char* file_path;
}RBEAssetTexture;

typedef struct RBEAssetFont {
    const char* file_path;
    const char* uid;
    size_t size;
}RBEAssetFont;

typedef struct RBEInputAction {
    const char* name;
    size_t valueCount;
    const char* values[4];
}RBEInputAction;

typedef struct RBEGameProperties {
    char gameTitle[32];
    int resolutionWidth;
    int resolutionHeight;
    int windowWidth;
    int windowHeight;
    int targetFPS;
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
