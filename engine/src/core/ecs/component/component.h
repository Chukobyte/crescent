#pragma once

#include <stdbool.h>

#include "../entity/entity.h"

#define MAX_COMPONENTS 4

typedef enum ComponentDataIndex {
    ComponentDataIndex_NONE = -1,
    ComponentDataIndex_TRANSFORM_2D = 0,
    ComponentDataIndex_SPRITE = 1,
    ComponentDataIndex_ANIMATED_SPRITE = 2,
    ComponentDataIndex_TEXT_LABEL = 3,
    ComponentDataIndex_SCRIPT = 4
} ComponentDataIndex;

typedef enum ComponentType {
    ComponentType_NONE = 0,
    ComponentType_TRANSFORM_2D = 1 << 0,
    ComponentType_SPRITE = 1 << 1,
    ComponentType_ANIMATED_SPRITE = 1 << 2,
    ComponentType_TEXT_LABEL = 1 << 3,
    ComponentType_SCRIPT = 1 << 4,
} ComponentType;

// --- Component Manager --- //
void component_manager_initialize();
void component_manager_finalize();
void* component_manager_get_component(Entity entity, ComponentDataIndex index);
void component_manager_set_component(Entity entity, ComponentDataIndex index, void* component);
void component_manager_remove_component(Entity entity, ComponentDataIndex index);
void component_manager_remove_all_components(Entity entity);
bool component_manager_has_component(Entity entity, ComponentDataIndex index);
void component_manager_set_component_signature(Entity entity, ComponentType componentTypeSignature);
ComponentType component_manager_get_component_signature(Entity entity);
