#pragma once

#include <stddef.h>

#include "../component/component.h"

typedef void (*OnEntityRegisteredFunc) (CreEntity); // init
typedef void (*OnEntityStartFunc) (CreEntity); // Useful for after initialization functionality (such as entering a scene)
typedef void (*OnEntityEndFunc) (CreEntity); // Useful for before deletion functionality (such as leaving a scene)
typedef void (*OnEntityUnRegisteredFunc) (CreEntity); // delete
typedef void (*OnEntityEnteredSceneFunc) (CreEntity); // When entity enters a scene (after _start() is called)
typedef void (*RenderFunc) ();
typedef void (*PreProcessAllFunc) ();
typedef void (*PostProcessAllFunc) ();
typedef void (*ProcessFunc) (float);
typedef void (*PhysicsProcessFunc) (float);
typedef void (*NetworkCallbackFunc) (const char*);

typedef struct CreEntitySystem {
    char* name;
    OnEntityRegisteredFunc on_entity_registered_func;
    OnEntityStartFunc on_entity_start_func;
    OnEntityEndFunc on_entity_end_func;
    OnEntityUnRegisteredFunc on_entity_unregistered_func;
    OnEntityEnteredSceneFunc on_entity_entered_scene_func;
    RenderFunc render_func;
    PreProcessAllFunc pre_process_all_func;
    PostProcessAllFunc post_process_all_func;
    ProcessFunc process_func;
    PhysicsProcessFunc physics_process_func;
    NetworkCallbackFunc network_callback_func;
    CreComponentType component_signature;
    size_t entity_count;
    CreEntity entities[CRE_MAX_ENTITIES];
} CreEntitySystem;

void cre_ec_system_initialize();
void cre_ec_system_finalize();
CreEntitySystem* cre_ec_system_create();
void cre_ec_system_destroy(CreEntitySystem* entitySystem);
void cre_ec_system_register(CreEntitySystem* system);
void cre_ec_system_update_entity_signature_with_systems(CreEntity entity);
void cre_ec_system_remove_entity_from_all_systems(CreEntity entity);
bool cre_ec_system_has_entity(CreEntity entity, CreEntitySystem* system);

void cre_ec_system_entity_start(CreEntity entity);
void cre_ec_system_entity_end(CreEntity entity);
void cre_ec_system_entity_entered_scene(CreEntity entity);
void cre_ec_system_render_systems();
void cre_ec_system_pre_process_all_systems();
void cre_ec_system_post_process_all_systems();
void cre_ec_system_process_systems(float deltaTime);
void cre_ec_system_physics_process_systems(float deltaTime);

void cre_ec_system_network_callback(const char* message);

// Entity System Management
CreEntity cre_ec_system_create_entity_uid();
void cre_ec_system_return_entity_uid(CreEntity entity);
