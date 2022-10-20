#include "engine_context.h"

#include <stddef.h>

#include "../seika/src/memory/rbe_mem.h"
#include "../seika/src/utils/rbe_assert.h"

RBEEngineContext* rbeEngineContext = NULL;

RBEEngineContext* rbe_engine_context_initialize() {
    RBE_ASSERT(rbeEngineContext == NULL);
    rbeEngineContext = RBE_MEM_ALLOCATE(RBEEngineContext);
    rbeEngineContext->isRunning = false;
    rbeEngineContext->targetFPS = 66;
    rbeEngineContext->averageFPS = 0.0f;
    rbeEngineContext->engineRootDir = NULL;
    rbeEngineContext->internalAssetsDir = NULL;
    return rbeEngineContext;
}

void rbe_engine_context_finalize() {
    RBE_ASSERT(rbeEngineContext != NULL);
    RBE_MEM_FREE(rbeEngineContext);
}

RBEEngineContext* rbe_engine_context_get() {
    return rbeEngineContext;
}
