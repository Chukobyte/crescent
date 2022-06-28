#pragma once

#include <stdbool.h>

typedef struct RBEEngineContext {
    bool isRunning;
    int targetFPS;
    float averageFPS;
} RBEEngineContext;

RBEEngineContext* rbe_engine_context_initialize();

void rbe_engine_context_finalize();

RBEEngineContext* rbe_engine_context_get();
