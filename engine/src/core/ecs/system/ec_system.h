#pragma once

#include <stddef.h>

#include "../component/component.h"

typedef void (*OnEntityRegisteredFunc) (Entity);
typedef void (*OnEntityUnRegisteredFunc) (Entity);
typedef void (*RenderFunc) ();
typedef void (*ProcessFunc) (float);
typedef void (*PhysicsProcessFunc) (float);

typedef enum EntitySystemHook {
    EntitySystemHook_NONE = 0,
    EntitySystemHook_RENDER = 1 << 0,
    EntitySystemHook_PROCESS = 1 << 1,
    EntitySystemHook_PHYSICS_PROCESS = 1 << 2
} EntitySystemHook;

typedef struct EntitySystem {
    OnEntityRegisteredFunc on_entity_registered_func;
    OnEntityUnRegisteredFunc on_entity_unregistered_func;
    RenderFunc render_func;
    ProcessFunc process_func;
    PhysicsProcessFunc physics_process_func;
    ComponentType component_signature;
    size_t entity_count;
    Entity entities[MAX_ENTITIES];
} EntitySystem;

void rbe_ec_system_initialize();
void rbe_ec_system_finalize();
EntitySystem* rbe_ec_system_create();
void rbe_ec_system_destroy(EntitySystem* entitySystem);
void rbe_ec_system_register(EntitySystem* system, EntitySystemHook systemHook);
void rbe_ec_system_register_entity_to_systems(Entity entity);
void rbe_ec_system_render_systems();
void rbe_ec_system_process_systems(float deltaTime);
void rbe_ec_system_physics_process_systems(float deltaTime);

// Entity System Management
Entity rbe_ec_system_create_entity();
