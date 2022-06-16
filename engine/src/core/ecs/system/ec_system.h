#pragma once

#include <stddef.h>

#include "../component/component.h"

typedef void (*OnEntityRegisteredFunc) (Entity); // init
typedef void (*OnEntityStartFunc) (Entity); // Useful for after initialization functionality (such as entering a scene)
typedef void (*OnEntityEndFunc) (Entity); // Useful for before deletion functionality (such as leaving a scene)
typedef void (*OnEntityUnRegisteredFunc) (Entity); // delete
typedef void (*RenderFunc) ();
typedef void (*ProcessFunc) (float);
typedef void (*PhysicsProcessFunc) (float);
typedef void (*NetworkCallbackFunc) (const char*);

typedef struct EntitySystem {
    char* name;
    OnEntityRegisteredFunc on_entity_registered_func;
    OnEntityStartFunc on_entity_start_func;
    OnEntityEndFunc on_entity_end_func;
    OnEntityUnRegisteredFunc on_entity_unregistered_func;
    RenderFunc render_func;
    ProcessFunc process_func;
    PhysicsProcessFunc physics_process_func;
    NetworkCallbackFunc network_callback_func;
    ComponentType component_signature;
    size_t entity_count;
    Entity entities[MAX_ENTITIES];
} EntitySystem;

void rbe_ec_system_initialize();
void rbe_ec_system_finalize();
EntitySystem* rbe_ec_system_create();
void rbe_ec_system_destroy(EntitySystem* entitySystem);
void rbe_ec_system_register(EntitySystem* system);
void rbe_ec_system_update_entity_signature_with_systems(Entity entity);
void rbe_ec_system_remove_entity_from_all_systems(Entity entity);
bool rbe_ec_system_has_entity(Entity entity, EntitySystem* system);

void rbe_ec_system_entity_start(Entity entity);
void rbe_ec_system_entity_end(Entity entity);
void rbe_ec_system_render_systems();
void rbe_ec_system_process_systems(float deltaTime);
void rbe_ec_system_physics_process_systems(float deltaTime);

void rbe_ec_system_network_callback(const char* message);

// Entity System Management
Entity rbe_ec_system_create_entity();
