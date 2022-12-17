#pragma once

#include <stdbool.h>

#define DEFAULT_START_PROJECT_PATH "test_games/cardboard_fighter"

#define CRE_GLOBAL_PHYSICS_DELTA_TIME 0.1f

typedef struct CreEngineStats {
    float averageFPS;
} CreEngineStats;

typedef struct CREEngineContext {
    bool isRunning;
    int targetFPS;
    char* engineRootDir;
    char* internalAssetsDir;
    char* projectArchivePath;
    CreEngineStats stats;
} CREEngineContext;

CREEngineContext* cre_engine_context_initialize();
void cre_engine_context_finalize();
CREEngineContext* cre_engine_context_get();
// Updates fps counter for now, will eventually add more and probably separate the stats api in the future
void cre_engine_context_update_stats();
