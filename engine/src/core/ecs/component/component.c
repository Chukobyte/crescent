#include "component.h"

#include <stddef.h>

#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"
#include "../../utils/logger.h"

// Component Array
typedef struct ComponentArray {
    void* components[MAX_COMPONENTS];
} ComponentArray;

ComponentArray* component_array_create() {
    ComponentArray* componentArray = RBE_MEM_ALLOCATE(ComponentArray);
    for (unsigned int i = 0; i < MAX_COMPONENTS; i++) {
        componentArray->components[i] = NULL;
    }
    return componentArray;
}

void component_array_initialize(ComponentArray* componentArray) {
    for (unsigned int i = 0; i < MAX_COMPONENTS; i++) {
        componentArray->components[i] = NULL;
    }
}

bool component_array_has_component(ComponentArray* componentArray, ComponentDataIndex index) {
    return componentArray->components[index] == NULL ? false : true;
}

void* component_array_get_component(ComponentArray* componentArray, ComponentDataIndex index) {
    RBE_ASSERT_FMT(component_array_has_component(componentArray, index), "Doesn't have component!");
    return componentArray->components[index];
}

void component_array_set_component(ComponentArray* componentArray, ComponentDataIndex index, void* component) {
    componentArray->components[index] = component;
}

void component_array_remove_component(ComponentArray* componentArray, ComponentDataIndex index) {
    componentArray->components[index] = NULL;
}

void component_array_finalize(ComponentArray* componentArray) {
    //    RBE_MEM_FREE(componentArray);
}

// Component Manager
typedef struct ComponentManager {
    ComponentArray entityComponentArrays[MAX_ENTITIES];
    ComponentType entityComponentSignatures[MAX_ENTITIES];
} ComponentManager;

static ComponentManager componentManager = {0};

ComponentType component_manager_translate_index_to_type(ComponentDataIndex index);

void component_manager_initialize() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        component_array_initialize(&componentManager.entityComponentArrays[i]);
        componentManager.entityComponentSignatures[i] = ComponentType_NONE;
    }
}

void component_manager_finalize() {}

void* component_manager_get_component(Entity entity, ComponentDataIndex index) {
    return component_array_get_component(&componentManager.entityComponentArrays[entity], index);
}

void component_manager_set_component(Entity entity, ComponentDataIndex index, void* component) {
    component_array_set_component(&componentManager.entityComponentArrays[entity], index, component);
    // Update signature
    ComponentType componentSignature = component_manager_get_component_signature(entity);
    componentSignature |= component_manager_translate_index_to_type(index);
    component_manager_set_component_signature(entity, componentSignature);
}

void component_manager_remove_component(Entity entity, ComponentDataIndex index) {
    void* component = &componentManager.entityComponentArrays[entity];
    ComponentType componentSignature = component_manager_get_component_signature(entity);
    componentSignature &= component_manager_translate_index_to_type(index);
    component_manager_set_component_signature(entity, componentSignature);
    component_array_remove_component(&componentManager.entityComponentArrays[entity], index);
    RBE_MEM_FREE(component);
}

bool component_manager_has_component(Entity entity, ComponentDataIndex index) {
    return component_array_has_component(&componentManager.entityComponentArrays[entity], index);
}

void component_manager_set_component_signature(Entity entity, ComponentType componentTypeSignature) {
    componentManager.entityComponentSignatures[entity] = componentTypeSignature;
}

ComponentType component_manager_get_component_signature(Entity entity) {
    return componentManager.entityComponentSignatures[entity];
}

ComponentType component_manager_translate_index_to_type(ComponentDataIndex index) {
    switch (index) {
    case ComponentDataIndex_TRANSFORM_2D:
        return ComponentType_TRANSFORM_2D;
    case ComponentDataIndex_SPRITE:
        return ComponentType_SPRITE;
    case ComponentDataIndex_TEXT_LABEL:
        return ComponentType_TEXT_LABEL;
    case ComponentDataIndex_NONE:
    default:
        rbe_logger_error("Not a valid component data index: '%d'", index);
        return ComponentType_NONE;
    }
}
