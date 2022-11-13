#include "engine_context.h"

#include <stddef.h>

#include "../seika/src/memory/se_mem.h"
#include "../seika/src/utils/se_assert.h"

CREEngineContext* creEngineContext = NULL;

CREEngineContext* cre_engine_context_initialize() {
    SE_ASSERT(creEngineContext == NULL);
    creEngineContext = SE_MEM_ALLOCATE(CREEngineContext);
    creEngineContext->isRunning = false;
    creEngineContext->targetFPS = 66;
    creEngineContext->averageFPS = 0.0f;
    creEngineContext->engineRootDir = NULL;
    creEngineContext->internalAssetsDir = NULL;
    creEngineContext->projectArchivePath = NULL;
    return creEngineContext;
}

void cre_engine_context_finalize() {
    SE_ASSERT(creEngineContext != NULL);
    if (creEngineContext->internalAssetsDir != NULL) {
        SE_MEM_FREE(creEngineContext->internalAssetsDir);
    }
    if (creEngineContext->projectArchivePath != NULL) {
        SE_MEM_FREE(creEngineContext->projectArchivePath);
    }
    SE_MEM_FREE(creEngineContext);
    creEngineContext = NULL;
}

CREEngineContext* cre_engine_context_get() {
    return creEngineContext;
}
