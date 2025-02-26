#include "tick.h"

#include <SDL3/SDL.h>

#include "seika/assert.h"
#include "seika/time.h"

typedef struct CreTick {
    CreTickUpdateFunc updateFunc;
    CreTickFixedUpdateFunc fixedUpdateFunc;
    uint64 currentTime;
    uint64 accumulator;
    uint32 updateInterval;
    uint32 fixedUpdateInterval;
    f32 fixedDeltaTime;
} CreTick;

static CreTick mainTick = {0};

void cre_tick_initialize(CreTickParams params) {
    mainTick.updateFunc = params.update;
    mainTick.fixedUpdateFunc = params.fixedUpdate;
    SKA_ASSERT(mainTick.updateFunc && mainTick.fixedUpdateFunc);
    mainTick.updateInterval = 1000 / params.targetFPS;
    mainTick.fixedUpdateInterval = 1000 / params.fixedTargetFPS;
    mainTick.fixedDeltaTime = (f32)mainTick.fixedUpdateInterval / 1000.0f;
    mainTick.currentTime = 0;
    mainTick.accumulator = 0;
}

void cre_tick_finalize() {
    mainTick = (CreTick){0};
}

void cre_tick_update() {
    const uint64 newTime = ska_get_ticks();
    const uint64 deltaTime = newTime - mainTick.currentTime;
    mainTick.currentTime = newTime;
    // Handle variable updates
    const f32 deltaTimeSeconds = (f32)deltaTime / 1000.f;
    mainTick.updateFunc(deltaTimeSeconds);
    // Handle fixed updates
    mainTick.accumulator += (uint64)mainTick.fixedDeltaTime;
    while (mainTick.accumulator >= mainTick.fixedUpdateInterval) {
        mainTick.fixedUpdateFunc(mainTick.fixedDeltaTime);
        mainTick.accumulator -= mainTick.fixedUpdateInterval;
    }
    // Apply delay to control frame rate based on target FPS
    const uint64 frameTime = ska_get_ticks() - mainTick.currentTime;
    if (frameTime < mainTick.updateInterval) {
        ska_delay(mainTick.updateInterval - frameTime);
    }
}
