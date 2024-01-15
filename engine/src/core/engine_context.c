#include "engine_context.h"

#include <seika/seika.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_assert.h>

#define MAX_FPS_SAMPLES 100
#define NUM_FRAMES_TO_AVERAGE 60

typedef struct CreFPSCounter {
    int tickIndex;
    int tickSum;
    int tickList[MAX_FPS_SAMPLES];
} CreFPSCounter;

typedef struct FPSCounter {
    int currentFrame;
    uint32_t frameTimes[NUM_FRAMES_TO_AVERAGE];
} FPSCounter;

CREEngineContext* creEngineContext = NULL;
//CreFPSCounter fpsCounter = { .tickIndex = 0, .tickSum = 0, .tickList = {0} };
FPSCounter fpsCounter = { .currentFrame = 0, .frameTimes = {0} };

CREEngineContext* cre_engine_context_initialize() {
    SE_ASSERT(creEngineContext == NULL);
    creEngineContext = SE_MEM_ALLOCATE(CREEngineContext);
    creEngineContext->isRunning = false;
    creEngineContext->targetFPS = 66;
    creEngineContext->stats.averageFPS = 0.0f;
    creEngineContext->engineRootDir = NULL;
    creEngineContext->internalAssetsDir = NULL;
    creEngineContext->projectArchivePath = NULL;
    creEngineContext->exitCode = EXIT_SUCCESS;

//    fpsCounter = (CreFPSCounter){ .tickIndex = 0, .tickSum = 0, .tickList = {0} };
    fpsCounter = (FPSCounter){ .currentFrame = 0, .frameTimes = {0} };
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

void cre_engine_context_update_stats(uint32_t frameTime) {
//    // Calculate average fps
//    static unsigned int countedFrames = 0;
//    if (countedFrames == 0) {
//        countedFrames = creEngineContext->targetFPS;
//    } else {
//        countedFrames++;
//    }
//    float averageFPS = (float) countedFrames / ((float) sf_get_ticks() / 1000.f);
//    creEngineContext->stats.averageFPS = averageFPS;

//    const int newTick = (int)sf_get_ticks();
//    fpsCounter.tickSum -= fpsCounter.tickList[fpsCounter.tickIndex];
//    fpsCounter.tickSum += newTick;
//    fpsCounter.tickList[fpsCounter.tickIndex] = newTick;
//    fpsCounter.tickIndex = (fpsCounter.tickIndex +1) % MAX_FPS_SAMPLES;
//
//    // Adjusted for the number of valid samples and avoiding division by zero
//    const float averageFrameTime = (fpsCounter.tickIndex == 0) ? 0.0f : (float)fpsCounter.tickSum / (float)fpsCounter.tickIndex;
//    creEngineContext->stats.averageFPS = (averageFrameTime > 0.0f) ? 1000.0f / averageFrameTime : 0.0f;

    fpsCounter.frameTimes[fpsCounter.currentFrame % NUM_FRAMES_TO_AVERAGE] = frameTime;
    fpsCounter.currentFrame++;

    uint32_t averageFrameTime = 0;
    for (int i = 0; i < NUM_FRAMES_TO_AVERAGE; i++) {
        averageFrameTime += fpsCounter.frameTimes[i];
    }
    averageFrameTime /= NUM_FRAMES_TO_AVERAGE;

    creEngineContext->stats.averageFPS = 1000.0f / (float)averageFrameTime;
}
