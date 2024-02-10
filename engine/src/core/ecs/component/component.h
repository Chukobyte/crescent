#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <seika/ecs/component.h>

#define CRE_MAX_COMPONENTS 10

//typedef enum CreComponentDataIndex {
//    CreComponentDataIndex_NONE = -1,
//    CreComponentDataIndex_NODE = 0,
//    CreComponentDataIndex_TRANSFORM_2D = 1,
//    CreComponentDataIndex_SPRITE = 2,
//    CreComponentDataIndex_ANIMATED_SPRITE = 3,
//    CreComponentDataIndex_TEXT_LABEL = 4,
//    CreComponentDataIndex_SCRIPT = 5,
//    CreComponentDataIndex_COLLIDER_2D = 6,
//    CreComponentDataIndex_COLOR_RECT = 7,
//    CreComponentDataIndex_PARALLAX = 8,
//    CreComponentDataIndex_PARTICLES_2D = 9,
//} CreComponentDataIndex;
//
//typedef enum CreComponentType {
//    CreComponentType_NONE = 0,
//    CreComponentType_NODE = 1 << 0,
//    CreComponentType_TRANSFORM_2D = 1 << 1,
//    CreComponentType_SPRITE = 1 << 2,
//    CreComponentType_ANIMATED_SPRITE = 1 << 3,
//    CreComponentType_TEXT_LABEL = 1 << 4,
//    CreComponentType_SCRIPT = 1 << 5,
//    CreComponentType_COLLIDER_2D = 1 << 6,
//    CreComponentType_COLOR_RECT = 1 << 7,
//    CreComponentType_PARALLAX = 1 << 8,
//    CreComponentType_PARTICLES_2D = 1 << 9,
//} CreComponentType;

// Struct that is used to pass information to observers from events
typedef struct CreComponentEntityUpdatePayload {
    SkaEntity entity;
    void* component;
//    CreComponentType componentType;
    SkaComponentType componentType;
} CreComponentEntityUpdatePayload;

// --- Component Manager --- //
//void cre_component_manager_initialize();
//void cre_component_manager_finalize();
//void* cre_component_manager_get_component(SkaEntity entity, CreComponentDataIndex index);
//void* cre_component_manager_get_component_unchecked(SkaEntity entity, CreComponentDataIndex index); // No check, will probably consolidate later...
//void cre_component_manager_set_component(SkaEntity entity, CreComponentDataIndex index, void* component);
//void cre_component_manager_remove_component(SkaEntity entity, CreComponentDataIndex index);
//void cre_component_manager_remove_all_components(SkaEntity entity);
//bool cre_component_manager_has_component(SkaEntity entity, CreComponentDataIndex index);
//void cre_component_manager_set_component_signature(SkaEntity entity, CreComponentType componentTypeSignature);
//CreComponentType cre_component_manager_get_component_signature(SkaEntity entity);
//
//const char* cre_component_get_component_data_index_string(CreComponentDataIndex index);

#ifdef __cplusplus
}
#endif
