#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <seika/ecs/component.h>

#define CRE_MAX_COMPONENTS 10

// Struct that is used to pass information to observers from events
typedef struct CreComponentEntityUpdatePayload {
    SkaEntity entity;
    void* component;
    SkaComponentType componentType;
} CreComponentEntityUpdatePayload;

#ifdef __cplusplus
}
#endif
