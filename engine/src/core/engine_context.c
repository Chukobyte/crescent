#include "engine_context.h"

#include <seika/seika.h>
#include <seika/memory.h>
#include <seika/assert.h>

#define MAX_FPS_SAMPLES 100
#define NUM_FRAMES_TO_AVERAGE 60

typedef struct CreFPSCounter {
    int32 tickIndex;
    int32 tickSum;
    int32 tickList[MAX_FPS_SAMPLES];
} CreFPSCounter;

typedef struct FPSCounter {
    int32 currentFrame;
    uint32_t frameTimes[NUM_FRAMES_TO_AVERAGE];
} FPSCounter;

CREEngineContext* creEngineContext = NULL;
//CreFPSCounter fpsCounter = { .tickIndex = 0, .tickSum = 0, .tickList = {0} };
FPSCounter fpsCounter = { .currentFrame = 0, .frameTimes = {0} };

CREEngineContext* cre_engine_context_initialize() {
    SKA_ASSERT(creEngineContext == NULL);
    creEngineContext = SKA_ALLOC(CREEngineContext);
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
    SKA_ASSERT(creEngineContext != NULL);
    if (creEngineContext->internalAssetsDir != NULL) {
        SKA_FREE(creEngineContext->internalAssetsDir);
    }
    if (creEngineContext->projectArchivePath != NULL) {
        SKA_FREE(creEngineContext->projectArchivePath);
    }
    SKA_FREE(creEngineContext);
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
//    float averageFPS = (float) countedFrames / ((float) ska_get_ticks() / 1000.f);
//    creEngineContext->stats.averageFPS = averageFPS;

//    const int newTick = (int)ska_get_ticks();
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
    uint32_t framesAboveZero = 0;
    for (int32 i = 0; i < NUM_FRAMES_TO_AVERAGE; i++) {
        if (fpsCounter.frameTimes[i] != 0) {
            averageFrameTime += fpsCounter.frameTimes[i];
            framesAboveZero++;
        }
//        averageFrameTime += fpsCounter.frameTimes[i];
    }
    SKA_ASSERT(framesAboveZero > 0);
    averageFrameTime /= framesAboveZero;
//    averageFrameTime /= NUM_FRAMES_TO_AVERAGE;

    creEngineContext->stats.averageFPS = 1000.0f / (f32)averageFrameTime;
}
