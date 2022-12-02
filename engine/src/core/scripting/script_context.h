#pragma once

#include "../ecs/entity/entity.h"

typedef void (*OnCreateInstance) (Entity, const char*, const char*);
typedef void (*OnDeleteInstance) (Entity);
typedef void (*OnStart) (Entity);

typedef void (*OnPreUpdateAll) ();
typedef void (*OnPostUpdateAll) ();
typedef void (*OnUpdateInstance) (Entity, float);
typedef void (*OnPhysicsUpdateInstance) (Entity, float);

typedef void (*OnUpdateAllInstances) (float);
typedef void (*OnPhysicsUpdateAllInstances) (float);
typedef void (*OnEnd) (Entity);
typedef void (*OnNetworkCallback) (const char*);

// TODO: Make not specific to python
struct _object; // PyObject
typedef void (*OnEntitySubscribeToNetworkCallback) (Entity, struct _object*, const char*);

// TODO: Add pre and post update callbacks
typedef struct CREScriptContext {
    OnCreateInstance on_create_instance;
    OnDeleteInstance on_delete_instance;
    // Called when an entity enters a scene
    OnStart on_start;
    // Called before all script update callbacks
    OnPreUpdateAll on_pre_update_all;
    // Called after all script update callbacks
    OnPostUpdateAll on_post_update_all;
    OnUpdateInstance on_update_instance;
    OnPhysicsUpdateInstance on_physics_update_instance;
    // Called before an entity exits a scene
    OnEnd on_end;
    // The main network callback for forwarding network data to the script context
    OnNetworkCallback on_network_callback;
    OnEntitySubscribeToNetworkCallback on_entity_subscribe_to_network_callback;
    // We could have a validation step on the script contexts to check if the update, physics_update, etc... funcs exists
    // in the class within the scripting language.  For now, the script context is responsible for entity and entity count
    // even though it's not used in the script ec system
    size_t updateEntityCount;
    size_t physicsUpdateEntityCount;
    Entity updateEntities[MAX_ENTITIES];
    Entity physicsUpdateEntities[MAX_ENTITIES];
} CREScriptContext;

CREScriptContext* cre_script_context_create();
