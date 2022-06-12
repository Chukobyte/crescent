#include "engine_context.h"

#include <stddef.h>

#include "memory/rbe_mem.h"
#include "utils/rbe_assert.h"

RBEEngineContext* rbeEngineContext = NULL;

RBEEngineContext* rbe_engine_context_initialize() {
    RBE_ASSERT(rbeEngineContext == NULL);
    rbeEngineContext = RBE_MEM_ALLOCATE(RBEEngineContext);
    rbeEngineContext->isRunning = false;
    rbeEngineContext->targetFPS = 66;
    rbeEngineContext->averageFPS = 0.0f;
    return rbeEngineContext;
}

void rbe_engine_context_finalize() {
    RBE_ASSERT(rbeEngineContext != NULL);
    RBE_MEM_FREE(rbeEngineContext);
}

RBEEngineContext* rbe_engine_context_get() {
    return rbeEngineContext;
}
