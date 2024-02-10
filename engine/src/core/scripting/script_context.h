#pragma once

#include <stddef.h>

#include <seika/ecs/entity.h>

typedef void (*OnCreateInstance) (SkaEntity, const char*, const char*);
typedef void (*OnDeleteInstance) (SkaEntity);
typedef void (*OnStart) (SkaEntity);
typedef void (*OnEnd) (SkaEntity);

typedef void (*OnPreUpdateAll) ();
typedef void (*OnPostUpdateAll) ();
typedef void (*OnUpdateInstance) (SkaEntity, float);
typedef void (*OnFixedUpdateInstance) (SkaEntity, float);

typedef void (*OnNetworkCallback) (const char*);

typedef void (*OnScriptContextDestroy) ();

// TODO: Make network callbacks not specific to python
struct _object; // PyObject
typedef void (*OnEntitySubscribeToNetworkCallback) (SkaEntity, struct _object*, const char*);

// Generic script context to be used as an interface for game scripting
// Note: Node event logics must be handled manually within the script context, refer to 'node_event.h' for the api.
// Also check out other script contexts for examples.
typedef struct CREScriptContext {
    // Called when a new entity instance is requested to be created once a node has entered the scene.
    // If using the same function to dynamically create instance from the script side, be sure to check if the instance
    // Isn't already created
    OnCreateInstance on_create_instance;
    // Called write before an entity is deleted from the scene
    OnDeleteInstance on_delete_instance;
    // Called when an entity enters a scene
    OnStart on_start;
    // Called before all script update callbacks
    OnPreUpdateAll on_pre_update_all;
    // Called after all script update callbacks
    OnPostUpdateAll on_post_update_all;
    OnUpdateInstance on_update_instance;
    OnFixedUpdateInstance on_fixed_update_instance;
    // Called before an entity exits a scene
    OnEnd on_end;
    // The main network callback for forwarding network data to the script context
    OnNetworkCallback on_network_callback;
//    OnEntitySubscribeToNetworkCallback on_entity_subscribe_to_network_callback;
    // Called when the script context is destroyed
    OnScriptContextDestroy on_script_context_destroy;
    // We could have a validation step on the script contexts to check if the update, fixed_update, etc... funcs exists
    // in the class within the scripting language.  For now, the script context is responsible for entity and entity count
    // even though it's not used in the script ec system
    size_t updateEntityCount;
    size_t fixedUpdateEntityCount;
    SkaEntity updateEntities[SKA_MAX_ENTITIES];
    SkaEntity fixedUpdateEntities[SKA_MAX_ENTITIES];
} CREScriptContext;

CREScriptContext* cre_script_context_create();
