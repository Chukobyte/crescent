#include "ec_system.h"

#include "../../data_structures/cre_queue.h"
#include "../../memory/rbe_mem.h"
#include "../../utils/logger.h"
#include "../../utils/rbe_assert.h"

//--- EC System Manager ---//
#define MAX_ENTITY_SYSTEMS_PER_HOOK 6

typedef struct EntitySystemData {
    size_t entity_systems_count;
    size_t on_entity_start_systems_count;
    size_t on_entity_end_systems_count;
    size_t render_systems_count;
    size_t process_systems_count;
    size_t physics_process_systems_count;
    size_t network_callback_systems_count;
    EntitySystem* entity_systems[MAX_COMPONENTS];
    EntitySystem* on_entity_start_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* on_entity_end_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* render_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* process_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* physics_process_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
    EntitySystem* network_callback_systems[MAX_ENTITY_SYSTEMS_PER_HOOK];
} EntitySystemData;

void rbe_ec_system_insert_entity_into_system(Entity entity, EntitySystem* system);
void rbe_ec_system_remove_entity_from_system(Entity entity, EntitySystem* system);

EntitySystemData entitySystemData;
CREQueue* entityIdQueue = NULL;

void rbe_ec_system_initialize() {
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        entitySystemData.entity_systems[i] = NULL;
    }
    for (size_t i = 0; i < MAX_ENTITY_SYSTEMS_PER_HOOK; i++) {
        entitySystemData.on_entity_start_systems[i] = NULL;
        entitySystemData.on_entity_end_systems[i] = NULL;
        entitySystemData.render_systems[i] = NULL;
        entitySystemData.process_systems[i] = NULL;
        entitySystemData.physics_process_systems[i] = NULL;
        entitySystemData.network_callback_systems[i] = NULL;
    }
    // Fill up entity id queue
    entityIdQueue = cre_queue_create(MAX_ENTITIES, NULL_ENTITY);
    for (Entity entityId = 0; entityId < MAX_ENTITIES; entityId++) {
        cre_queue_enqueue(entityIdQueue, entityId + 1);
    }
    // Set system data initial values
    entitySystemData.entity_systems_count = 0;
    entitySystemData.on_entity_start_systems_count = 0;
    entitySystemData.on_entity_end_systems_count = 0;
    entitySystemData.render_systems_count = 0;
    entitySystemData.process_systems_count = 0;
    entitySystemData.physics_process_systems_count = 0;
    entitySystemData.network_callback_systems_count = 0;
}

void rbe_ec_system_finalize() {
    for (size_t i = 0; i < entitySystemData.entity_systems_count; i++) {
        rbe_ec_system_destroy(entitySystemData.entity_systems[i]);
        entitySystemData.entity_systems[i] = NULL;
    }
    cre_queue_destroy(entityIdQueue);
}

EntitySystem* rbe_ec_system_create() {
    EntitySystem* newSystem = RBE_MEM_ALLOCATE(EntitySystem);
    newSystem->name = NULL;
    newSystem->entity_count = 0;
    newSystem->on_entity_registered_func = NULL;
    newSystem->on_entity_start_func = NULL;
    newSystem->on_entity_end_func = NULL;
    newSystem->on_entity_unregistered_func = NULL;
    newSystem->render_func = NULL;
    newSystem->process_func = NULL;
    newSystem->physics_process_func = NULL;
    newSystem->network_callback_func = NULL;
    newSystem->component_signature = ComponentType_NONE;
    return newSystem;
}

void rbe_ec_system_destroy(EntitySystem* entitySystem) {
    RBE_MEM_FREE(entitySystem);
}

void rbe_ec_system_register(EntitySystem* system) {
    RBE_ASSERT_FMT(system != NULL, "Passed in system is NULL!");
    entitySystemData.entity_systems[entitySystemData.entity_systems_count++] = system;
    if (system->on_entity_start_func != NULL) {
        entitySystemData.on_entity_start_systems[entitySystemData.on_entity_start_systems_count++] = system;
    }
    if (system->on_entity_end_func != NULL) {
        entitySystemData.on_entity_end_systems[entitySystemData.on_entity_end_systems_count++] = system;
    }
    if (system->render_func != NULL) {
        entitySystemData.render_systems[entitySystemData.render_systems_count++] = system;
    }
    if (system->process_func != NULL) {
        entitySystemData.process_systems[entitySystemData.process_systems_count++] = system;
    }
    if (system->physics_process_func != NULL) {
        entitySystemData.physics_process_systems[entitySystemData.physics_process_systems_count++] = system;
    }
    if (system->network_callback_func != NULL) {
        entitySystemData.network_callback_systems[entitySystemData.network_callback_systems_count++] = system;
    }
}

void rbe_ec_system_update_entity_signature_with_systems(Entity entity) {
    ComponentType entityComponentSignature = component_manager_get_component_signature(entity);
    for (size_t i = 0; i < entitySystemData.entity_systems_count; i++) {
        if ((entityComponentSignature & entitySystemData.entity_systems[i]->component_signature) == entitySystemData.entity_systems[i]->component_signature) {
            rbe_ec_system_insert_entity_into_system(entity, entitySystemData.entity_systems[i]);
        } else {
            rbe_ec_system_remove_entity_from_system(entity, entitySystemData.entity_systems[i]);
        }
    }
}

void rbe_ec_system_entity_start(Entity entity) {
    ComponentType entityComponentSignature = component_manager_get_component_signature(entity);
    for (size_t i = 0; i < entitySystemData.on_entity_start_systems_count; i++) {
        if ((entityComponentSignature & entitySystemData.on_entity_start_systems[i]->component_signature) == entitySystemData.on_entity_start_systems[i]->component_signature) {
            entitySystemData.on_entity_start_systems[i]->on_entity_start_func(entity);
        }
    }
}

void rbe_ec_system_entity_end(Entity entity) {
    ComponentType entityComponentSignature = component_manager_get_component_signature(entity);
    for (size_t i = 0; i < entitySystemData.on_entity_end_systems_count; i++) {
        if ((entityComponentSignature & entitySystemData.on_entity_end_systems[i]->component_signature) == entitySystemData.on_entity_end_systems[i]->component_signature) {
            entitySystemData.on_entity_end_systems[i]->on_entity_end_func(entity);
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

void rbe_ec_system_network_callback(const char* message) {
    for (size_t i = 0; i < entitySystemData.network_callback_systems_count; i++) {
        entitySystemData.network_callback_systems[i]->network_callback_func(message);
    }
}

// --- Entity Management --- //
Entity cre_ec_system_create_entity_uid() {
    RBE_ASSERT_FMT(!cre_queue_is_empty(entityIdQueue), "Entity pool is empty.  Entity limit reached, considering increasing MAX_ENTITIES!");
    return cre_queue_dequeue(entityIdQueue);
}

void cre_ec_system_return_entity_uid(Entity entity) {
    cre_queue_enqueue(entityIdQueue, entity);
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
        system->entities[system->entity_count++] = entity;
        if (system->on_entity_registered_func != NULL) {
            system->on_entity_registered_func(entity);
        }
    } else {
        rbe_logger_warn("Entity '%d' already in system '%s'", entity, system->name);
    }
}

void rbe_ec_system_remove_entity_from_system(Entity entity, EntitySystem* system) {
    for (size_t i = 0; i < system->entity_count; i++) {
        if (entity == system->entities[i]) {
            // Entity found
            if (i + 1 < MAX_ENTITIES) {
                system->entities[i] = NULL_ENTITY;
            }
            if (system->on_entity_unregistered_func != NULL) {
                system->on_entity_unregistered_func(entity);
            }

            // Condense array
            for (size_t newIndex = i; i < system->entity_count; i++) {
                if (system->entities[i] == NULL_ENTITY) {
                    // Early exit if 2 consecutive nulls
                    if (system->entities[i + 1] == NULL_ENTITY) {
                        break;
                    }
                    system->entities[i] = system->entities[i + 1];
                    system->entities[i + 1] = NULL_ENTITY;
                }
            }

            system->entity_count--;
            break;
        }
    }
}

void rbe_ec_system_remove_entity_from_all_systems(Entity entity) {
    for (size_t i = 0; i < entitySystemData.entity_systems_count; i++) {
        rbe_ec_system_remove_entity_from_system(entity, entitySystemData.entity_systems[i]);
    }
}
