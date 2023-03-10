#include "component.h"

#include <stddef.h>

#include "../seika/src/memory/se_mem.h"
#include "../seika/src/utils/se_assert.h"

//--- Component Array ---//
typedef struct ComponentArray {
    void* components[CRE_MAX_COMPONENTS];
} ComponentArray;

ComponentArray* component_array_create() {
    ComponentArray* componentArray = SE_MEM_ALLOCATE(ComponentArray);
    for (unsigned int i = 0; i < CRE_MAX_COMPONENTS; i++) {
        componentArray->components[i] = NULL;
    }
    return componentArray;
}

void component_array_initialize(ComponentArray* componentArray) {
    for (unsigned int i = 0; i < CRE_MAX_COMPONENTS; i++) {
        componentArray->components[i] = NULL;
    }
}

bool component_array_has_component(ComponentArray* componentArray, CreComponentDataIndex index) {
    return componentArray->components[index] == NULL ? false : true;
}

void* component_array_get_component(ComponentArray* componentArray, CreComponentDataIndex index) {
    return componentArray->components[index];
}

void component_array_set_component(ComponentArray* componentArray, CreComponentDataIndex index, void* component) {
    componentArray->components[index] = component;
}

void component_array_remove_component(ComponentArray* componentArray, CreComponentDataIndex index) {
    if (component_array_has_component(componentArray, index)) {
        SE_MEM_FREE(componentArray->components[index]);
        componentArray->components[index] = NULL;
    }
}

void component_array_remove_all_components(ComponentArray* componentArray) {
    for (size_t i = 0; i < CRE_MAX_COMPONENTS; i++) {
        component_array_remove_component(componentArray, (CreComponentDataIndex) i);
    }
}

//--- Component Manager ---//
typedef struct ComponentManager {
    ComponentArray* entityComponentArrays[CRE_MAX_ENTITIES];
    CreComponentType entityComponentSignatures[CRE_MAX_ENTITIES];
} ComponentManager;

static ComponentManager* componentManager = NULL;

CreComponentType component_manager_translate_index_to_type(CreComponentDataIndex index);

void cre_component_manager_initialize() {
    SE_ASSERT_FMT(componentManager == NULL, "Component Manager is not NULL when trying to initialize");
    componentManager = SE_MEM_ALLOCATE(ComponentManager);
    for (int i = 0; i < CRE_MAX_ENTITIES; i++) {
        componentManager->entityComponentArrays[i] = SE_MEM_ALLOCATE(ComponentArray);
        component_array_initialize(componentManager->entityComponentArrays[i]);
        componentManager->entityComponentSignatures[i] = CreComponentType_NONE;
    }
}

// Assumes component data was already deleted previously
void cre_component_manager_finalize() {
    SE_ASSERT_FMT(componentManager != NULL, "Component Manager is NULL when trying to finalize...");
    SE_MEM_FREE(componentManager);
    componentManager = NULL;
}

void* cre_component_manager_get_component(CreEntity entity, CreComponentDataIndex index) {
    void* component = component_array_get_component(componentManager->entityComponentArrays[entity], index);
    SE_ASSERT_FMT(component != NULL, "Entity '%d' doesn't have '%s' component!",
                  entity, cre_component_get_component_data_index_string(index));
    return component;
}

void* cre_component_manager_get_component_unchecked(CreEntity entity, CreComponentDataIndex index) {
    return component_array_get_component(componentManager->entityComponentArrays[entity], index);
}

void cre_component_manager_set_component(CreEntity entity, CreComponentDataIndex index, void* component) {
//    RBE_ASSERT_FMT(entity < MAX_ENTITIES, "Entity limit reached!  Try increasing MAX_ENTITIES count or implementing entity id pooling!");
    component_array_set_component(componentManager->entityComponentArrays[entity], index, component);
    // Update signature
    CreComponentType componentSignature = cre_component_manager_get_component_signature(entity);
    componentSignature |= component_manager_translate_index_to_type(index);
    cre_component_manager_set_component_signature(entity, componentSignature);
}

void cre_component_manager_remove_component(CreEntity entity, CreComponentDataIndex index) {
    CreComponentType componentSignature = cre_component_manager_get_component_signature(entity);
    componentSignature &= component_manager_translate_index_to_type(index);
    cre_component_manager_set_component_signature(entity, componentSignature);
    component_array_remove_component(componentManager->entityComponentArrays[entity], index);
}

void cre_component_manager_remove_all_components(CreEntity entity) {
    component_array_remove_all_components(componentManager->entityComponentArrays[entity]);
    cre_component_manager_set_component_signature(entity, CreComponentType_NONE);
}

bool cre_component_manager_has_component(CreEntity entity, CreComponentDataIndex index) {
    return component_array_has_component(componentManager->entityComponentArrays[entity], index);
}

void cre_component_manager_set_component_signature(CreEntity entity, CreComponentType componentTypeSignature) {
    componentManager->entityComponentSignatures[entity] = componentTypeSignature;
}

CreComponentType cre_component_manager_get_component_signature(CreEntity entity) {
    return componentManager->entityComponentSignatures[entity];
}

CreComponentType component_manager_translate_index_to_type(CreComponentDataIndex index) {
    switch (index) {
    case CreComponentDataIndex_NODE:
        return CreComponentType_NODE;
    case CreComponentDataIndex_TRANSFORM_2D:
        return CreComponentType_TRANSFORM_2D;
    case CreComponentDataIndex_SPRITE:
        return CreComponentType_SPRITE;
    case CreComponentDataIndex_ANIMATED_SPRITE:
        return CreComponentType_ANIMATED_SPRITE;
    case CreComponentDataIndex_TEXT_LABEL:
        return CreComponentType_TEXT_LABEL;
    case CreComponentDataIndex_SCRIPT:
        return CreComponentType_SCRIPT;
    case CreComponentDataIndex_COLLIDER_2D:
        return CreComponentType_COLLIDER_2D;
    case CreComponentDataIndex_COLOR_RECT:
        return CreComponentType_COLOR_RECT;
    case CreComponentDataIndex_PARALLAX:
        return CreComponentType_PARALLAX;
    case CreComponentDataIndex_NONE:
    default:
        se_logger_error("Not a valid component data index: '%d'", index);
        return CreComponentType_NONE;
    }
}

const char* cre_component_get_component_data_index_string(CreComponentDataIndex index) {
    switch (index) {
    case CreComponentDataIndex_NODE:
        return "Node";
    case CreComponentDataIndex_TRANSFORM_2D:
        return "Transform2D";
    case CreComponentDataIndex_SPRITE:
        return "Sprite";
    case CreComponentDataIndex_ANIMATED_SPRITE:
        return "Animated Sprite";
    case CreComponentDataIndex_TEXT_LABEL:
        return "Text Label";
    case CreComponentDataIndex_SCRIPT:
        return "Script";
    case CreComponentDataIndex_COLLIDER_2D:
        return "Collider2D";
    case CreComponentDataIndex_COLOR_RECT:
        return "ColorRect";
    case CreComponentDataIndex_PARALLAX:
        return "Parallax";
    case CreComponentDataIndex_NONE:
    default:
        se_logger_error("Not a valid component data index: '%d'", index);
        return "NONE";
    }
    return "INVALID";
}
