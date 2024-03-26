#pragma once

#include <stdbool.h>

#include <seika/defines.h>

#define DEFAULT_START_PROJECT_PATH "test_games/cardboard_fighter"

#define CRE_GLOBAL_PHYSICS_DELTA_TIME 0.1f
#define CRE_DEFAULT_FONT_KEY "_default"

typedef struct CreEngineStats {
    f32 averageFPS;
} CreEngineStats;

typedef struct CREEngineContext {
    bool isRunning;
    int32 targetFPS;
    // Root directory for engine, where the executable binary is
    char* engineRootDir;
    // Where scripts, assets, in all other things are expected to be.  Also known as project directory.
    char* internalAssetsDir;
    // Where the project archive file is located (used when reading assets from memory in shipping builds)
    char* projectArchivePath;
    CreEngineStats stats;
    int32 exitCode;
} CREEngineContext;

CREEngineContext* cre_engine_context_initialize();
void cre_engine_context_finalize();
CREEngineContext* cre_engine_context_get();
// Updates fps counter for now, will eventually add more and probably separate the stats api in the future
void cre_engine_context_update_stats(uint32_t frameTime);
