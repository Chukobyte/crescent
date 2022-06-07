#include "ec_system.h"

#include "../../memory/rbe_mem.h"
#include "../../utils/logger.h"
#include "../../utils/rbe_assert.h"

#define MAX_ENTITY_SYSTEMS_PER_HOOK 4

typedef struct EntitySystemData {
    size_t entity_systems_count;
    size_t render_systems_count;
    size_t process_systems_count;
    size_t physics_process_systems_count;
    EntitySystem* entity_systems[MAX_COMPONENTS];
    EntitySystem* render_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* process_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* physics_process_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
} EntitySystemData;

bool rbe_ec_system_has_entity(Entity entity, EntitySystem* system);
void rbe_ec_system_insert_entity_into_system(Entity entity, EntitySystem* system);
bool rbe_ec_system_remove_entity_from_system(Entity entity, EntitySystem* system);

EntitySystemData entitySystemData;
Entity entityIndex = 1; // 0 is NULL_ENTITY

void rbe_ec_system_initialize() {
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        entitySystemData.entity_systems[i] = NULL;
    }
    for (size_t i = 0; i < MAX_ENTITY_SYSTEMS_PER_HOOK; i++) {
        entitySystemData.render_systems[i] = NULL;
        entitySystemData.process_systems[i] = NULL;
        entitySystemData.physics_process_systems[i] = NULL;
    }
}

void rbe_ec_system_finalize() {
    for (size_t i = 0; i < entitySystemData.entity_systems_count; i++) {
        rbe_ec_system_destroy(entitySystemData.entity_systems[i]);
        entitySystemData.entity_systems[i] = NULL;
    }
    entitySystemData.entity_systems_count = 0;
    entitySystemData.render_systems_count = 0;
    entitySystemData.process_systems_count = 0;
    entitySystemData.physics_process_systems_count = 0;
}

EntitySystem* rbe_ec_system_create() {
    EntitySystem* newSystem = RBE_MEM_ALLOCATE(EntitySystem);
    newSystem->entity_count = 0;
    newSystem->on_entity_registered_func = NULL;
    newSystem->on_entity_unregistered_func = NULL;
    newSystem->render_func = NULL;
    newSystem->process_func = NULL;
    newSystem->physics_process_func = NULL;
    newSystem->component_signature = ComponentType_NONE;
    return newSystem;
}

void rbe_ec_system_destroy(EntitySystem* entitySystem) {
    RBE_MEM_FREE(entitySystem);
}

void rbe_ec_system_register(EntitySystem* system) {
    RBE_ASSERT_FMT(system != NULL, "Passed in system is NULL!");
    entitySystemData.entity_systems[entitySystemData.entity_systems_count++] = system;
    if (system->render_func != NULL) {
        entitySystemData.render_systems[entitySystemData.render_systems_count++] = system;
    }
    if (system->process_func != NULL) {
        entitySystemData.process_systems[entitySystemData.process_systems_count++] = system;
    }
    if (system->physics_process_func != NULL) {
        entitySystemData.physics_process_systems[entitySystemData.physics_process_systems_count++] = system;
    }
}

void rbe_ec_system_register_entity_to_systems(Entity entity) {
    ComponentType entityComponentSignature = component_manager_get_component_signature(entity);
    rbe_logger_debug("entityComponentSignature = %d", entityComponentSignature);
    for (size_t i = 0; i < entitySystemData.entity_systems_count; i++) {
        if ((entityComponentSignature & entitySystemData.entity_systems[i]->component_signature) == entityComponentSignature) {
            rbe_ec_system_insert_entity_into_system(entity, entitySystemData.entity_systems[i]);
        } else {
            rbe_ec_system_remove_entity_from_system(entity, entitySystemData.entity_systems[i]);
        }
    }
}

void rbe_ec_system_render_systems() {
    for (size_t i = 0; i < entitySystemData.render_systems_count; i++) {
        entitySystemData.render_systems[i]->render_func();
    }
}

void rbe_ec_system_process_systems(float deltaTime) {
    for (size_t i = 0; i < entitySystemData.process_systems_count; i++) {
        entitySystemData.process_systems[i]->process_func(deltaTime);
    }
}

void rbe_ec_system_physics_process_systems(float deltaTime) {
    for (size_t i = 0; i < entitySystemData.physics_process_systems_count; i++) {
        entitySystemData.physics_process_systems[i]->physics_process_func(deltaTime);
    }
}

// --- Entity Management --- //
Entity rbe_ec_system_create_entity() {
    // TODO: Create entity pool
    Entity newEntity = entityIndex;
    entityIndex++;
    return newEntity;
}

// --- Internal Functions --- //
bool rbe_ec_system_has_entity(Entity entity, EntitySystem* system) {
    for (size_t i = 0; i < system->entity_count; i++) {
        if (entity == system->entities[i]) {
            return true;
        }
    }
    return false;
}

void rbe_ec_system_insert_entity_into_system(Entity entity, EntitySystem* system) {
    if (!rbe_ec_system_has_entity(entity, system)) {
        system->entities[system->entity_count] = entity;
        if (system->on_entity_registered_func != NULL) {
            system->on_entity_registered_func(entity);
        }
        system->entity_count++;
    } else {
        rbe_logger_error("Entity already in system!");
    }
}

bool rbe_ec_system_remove_entity_from_system(Entity entity, EntitySystem* system) {
    for (size_t i = 0; i < system->entity_count; i++) {
        if (entity == system->entities[i]) {
            // Entity found
            if (system->entity_count <= 1) {
                system->entities[0] = NULL_ENTITY;
            } else if (i != system->entity_count - 1) {
                // Swaps from front to place removed unless at the end
                system->entities[i] = system->entities[i + 1];
            }
            if (system->on_entity_unregistered_func != NULL) {
                system->on_entity_unregistered_func(entity);
            }
            system->entity_count--;
            return true;
        }
    }
    return false;
}
