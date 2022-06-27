#include "component.h"

#include <stddef.h>

#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"

//--- Component Array ---//
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
    return componentArray->components[index];
}

void component_array_set_component(ComponentArray* componentArray, ComponentDataIndex index, void* component) {
    componentArray->components[index] = component;
}

void component_array_remove_component(ComponentArray* componentArray, ComponentDataIndex index) {
    if (component_array_has_component(componentArray, index)) {
        RBE_MEM_FREE(componentArray->components[index]);
        componentArray->components[index] = NULL;
    }
}

void component_array_remove_all_components(ComponentArray* componentArray) {
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        component_array_remove_component(componentArray, (ComponentDataIndex) i);
    }
}

//--- Component Manager ---//
typedef struct ComponentManager {
    ComponentArray* entityComponentArrays[MAX_ENTITIES];
    ComponentType entityComponentSignatures[MAX_ENTITIES];
} ComponentManager;

static ComponentManager* componentManager = NULL;

ComponentType component_manager_translate_index_to_type(ComponentDataIndex index);

void component_manager_initialize() {
    RBE_ASSERT(componentManager == NULL);
    componentManager = RBE_MEM_ALLOCATE(ComponentManager);
    for (int i = 0; i < MAX_ENTITIES; i++) {
        componentManager->entityComponentArrays[i] = RBE_MEM_ALLOCATE(ComponentArray);
        component_array_initialize(componentManager->entityComponentArrays[i]);
        componentManager->entityComponentSignatures[i] = ComponentType_NONE;
    }
}

void component_manager_finalize() {}

void* component_manager_get_component(Entity entity, ComponentDataIndex index) {
    void* component = component_array_get_component(componentManager->entityComponentArrays[entity], index);
    RBE_ASSERT_FMT(component != NULL, "Entity '%d' doesn't have '%s' component!",
                   entity, component_get_component_data_index_string(index));
    return component;
}

void* component_manager_get_component_unsafe(Entity entity, ComponentDataIndex index) {
    return component_array_get_component(componentManager->entityComponentArrays[entity], index);
}

void component_manager_set_component(Entity entity, ComponentDataIndex index, void* component) {
    component_array_set_component(componentManager->entityComponentArrays[entity], index, component);
    // Update signature
    ComponentType componentSignature = component_manager_get_component_signature(entity);
    componentSignature |= component_manager_translate_index_to_type(index);
    component_manager_set_component_signature(entity, componentSignature);
}

void component_manager_remove_component(Entity entity, ComponentDataIndex index) {
    ComponentType componentSignature = component_manager_get_component_signature(entity);
    componentSignature &= component_manager_translate_index_to_type(index);
    component_manager_set_component_signature(entity, componentSignature);
    component_array_remove_component(componentManager->entityComponentArrays[entity], index);
}

void component_manager_remove_all_components(Entity entity) {
    component_array_remove_all_components(componentManager->entityComponentArrays[entity]);
}

bool component_manager_has_component(Entity entity, ComponentDataIndex index) {
    return component_array_has_component(componentManager->entityComponentArrays[entity], index);
}

void component_manager_set_component_signature(Entity entity, ComponentType componentTypeSignature) {
    componentManager->entityComponentSignatures[entity] = componentTypeSignature;
}

ComponentType component_manager_get_component_signature(Entity entity) {
    return componentManager->entityComponentSignatures[entity];
}

ComponentType component_manager_translate_index_to_type(ComponentDataIndex index) {
    switch (index) {
    case ComponentDataIndex_NODE:
        return ComponentType_NODE;
    case ComponentDataIndex_TRANSFORM_2D:
        return ComponentType_TRANSFORM_2D;
    case ComponentDataIndex_SPRITE:
        return ComponentType_SPRITE;
    case ComponentDataIndex_ANIMATED_SPRITE:
        return ComponentType_ANIMATED_SPRITE;
    case ComponentDataIndex_TEXT_LABEL:
        return ComponentType_TEXT_LABEL;
    case ComponentDataIndex_SCRIPT:
        return ComponentType_SCRIPT;
    case ComponentDataIndex_COLLIDER_2D:
        return ComponentType_COLLIDER_2D;
    case ComponentDataIndex_NONE:
    default:
        rbe_logger_error("Not a valid component data index: '%d'", index);
        return ComponentType_NONE;
    }
}

const char* component_get_component_data_index_string(ComponentDataIndex index) {
    switch (index) {
    case ComponentDataIndex_NODE:
        return "Node";
    case ComponentDataIndex_TRANSFORM_2D:
        return "Transform2D";
    case ComponentDataIndex_SPRITE:
        return "Sprite";
    case ComponentDataIndex_ANIMATED_SPRITE:
        return "Animated Sprite";
    case ComponentDataIndex_TEXT_LABEL:
        return "Text Label";
    case ComponentDataIndex_SCRIPT:
        return "Script";
    case ComponentDataIndex_COLLIDER_2D:
        return "Collider2D";
    case ComponentDataIndex_NONE:
    default:
        rbe_logger_error("Not a valid component data index: '%d'", index);
        return "NONE";
    }
    return "INVALID";
}
