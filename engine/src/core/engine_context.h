#pragma once

#include <stdbool.h>

//#define DEFAULT_START_PROJECT_PATH "test_games/cardboard_fighter"
#define DEFAULT_START_PROJECT_PATH "test_games/dec_jam"

typedef struct CREEngineContext {
    bool isRunning;
    int targetFPS;
    float averageFPS;
    char* engineRootDir;
    char* internalAssetsDir;
    char* projectArchivePath;
} CREEngineContext;

CREEngineContext* cre_engine_context_initialize();
void cre_engine_context_finalize();
CREEngineContext* cre_engine_context_get();
