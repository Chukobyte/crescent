#pragma once

#include <stdbool.h>

#define DEFAULT_START_PROJECT_PATH "test_games/cardboard_fighter"

typedef struct RBEEngineContext {
    bool isRunning;
    int targetFPS;
    float averageFPS;
    char* engineRootDir;
    char* internalAssetsDir;
} RBEEngineContext;

RBEEngineContext* rbe_engine_context_initialize();
void rbe_engine_context_finalize();
RBEEngineContext* rbe_engine_context_get();
