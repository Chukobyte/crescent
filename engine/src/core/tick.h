#pragma once

#include <seika/defines.h>

#define CRE_MAX_TICKS 8
#define CRE_INVALID_HANDLE (CreTickHandle)-1

typedef void (*CreTickUpdateFunc) (f32);
typedef void (*CreTickFixedUpdateFunc) (f32);

typedef struct CreTickParams {
    uint32 targetFPS;
    uint32 fixedTargetFPS;
    CreTickUpdateFunc update;
    CreTickFixedUpdateFunc fixedUpdate;
} CreTickParams;

void cre_tick_initialize(CreTickParams params);
void cre_tick_finalize();
void cre_tick_update();
