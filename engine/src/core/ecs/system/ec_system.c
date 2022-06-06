#include "ec_system.h"

#include "../../utils/logger.h"

#define MAX_ENTITY_SYSTEMS_PER_HOOK 4

typedef struct EntitySystemData {
    size_t entitySystemsCount;
    size_t renderSystemsCount;
    size_t processSystemsCount;
    size_t physicsProcessSystemsCount;
    EntitySystem* entitySystems[MAX_COMPONENTS];
    EntitySystem* renderSystems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* processSystems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* physicsProcessSystems[MAX_ENTITY_SYSTEMS_PER_HOOK];
} EntitySystemData;

bool rbe_ec_system_has_entity(Entity entity, EntitySystem* system);
void rbe_ec_system_insert_entity_into_system(Entity entity, EntitySystem* system);
bool rbe_ec_system_remove_entity_from_system(Entity entity, EntitySystem* system);

EntitySystemData entitySystemData;
Entity entityIndex = 1; // 0 is NULL_ENTITY

void rbe_ec_system_initialize() {
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        entitySystemData.entitySystems[i] = NULL;
    }
    for (size_t i = 0; i < MAX_ENTITY_SYSTEMS_PER_HOOK; i++) {
        entitySystemData.renderSystems[i] = NULL;
        entitySystemData.processSystems[i] = NULL;
        entitySystemData.physicsProcessSystems[i] = NULL;
    }
}

void rbe_ec_system_finalize() {}

void rbe_ec_system_register(EntitySystem* system, EntitySystemHook systemHook) {
    entitySystemData.entitySystems[entitySystemData.entitySystemsCount] = system;
    entitySystemData.entitySystemsCount++;
    if (systemHook & EntitySystemHook_RENDER) {
        entitySystemData.renderSystems[entitySystemData.renderSystemsCount] = system;
        entitySystemData.renderSystemsCount++;
    }
    if (systemHook & EntitySystemHook_PROCESS) {
        entitySystemData.processSystems[entitySystemData.processSystemsCount] = system;
        entitySystemData.processSystemsCount++;
    }
    if (systemHook & EntitySystemHook_PHYSICS_PROCESS) {
        entitySystemData.physicsProcessSystems[entitySystemData.physicsProcessSystemsCount] = system;
        entitySystemData.physicsProcessSystemsCount++;
    }
}

void rbe_ec_system_register_entity_to_systems(Entity entity) {
    ComponentType entityComponentSignature = component_manager_get_component_signature(entity);
    for (size_t i = 0; i < entitySystemData.entitySystemsCount; i++) {
        if ((entityComponentSignature & entitySystemData.entitySystems[i]->component_signature) == entityComponentSignature) {
            rbe_ec_system_insert_entity_into_system(entity, entitySystemData.entitySystems[i]);
        } else {
            rbe_ec_system_remove_entity_from_system(entity, entitySystemData.entitySystems[i]);
        }
    }
}

void rbe_ec_system_render_systems() {
    for (size_t i = 0; i < entitySystemData.renderSystemsCount; i++) {
        entitySystemData.renderSystems[i]->render_func();
    }
}

void rbe_ec_system_process_systems(float deltaTime) {
    for (size_t i = 0; i < entitySystemData.processSystemsCount; i++) {
        entitySystemData.processSystems[i]->process_func(deltaTime);
    }
}

void rbe_ec_system_physics_process_systems(float deltaTime) {
    for (size_t i = 0; i < entitySystemData.physicsProcessSystemsCount; i++) {
        entitySystemData.physicsProcessSystems[i]->physics_process_func(deltaTime);
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
        system->on_entity_registered_func(entity);
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
            system->on_entity_unregistered_func(entity);
            system->entity_count--;
            return true;
        }
    }
    return false;
}
