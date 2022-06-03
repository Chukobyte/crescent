#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>

typedef struct RBEGameProperties {
    char gameTitle[32];
    int resolutionWidth;
    int resolutionHeight;
    int windowWidth;
    int windowHeight;
    int targetFPS;
} RBEGameProperties;

void rbe_game_props_initialize(bool loadConfig);
void rbe_game_props_finalize();
RBEGameProperties* rbe_game_props_get();
RBEGameProperties rbe_game_props_read_file(const char* filePath);
void rbe_game_props_print();

#ifdef __cplusplus
}
#endif
