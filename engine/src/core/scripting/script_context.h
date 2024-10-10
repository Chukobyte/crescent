#pragma once

#include <stddef.h>

#include <seika/ecs/entity.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*OnCreateInstance) (SkaEntity, const char*, const char*);
typedef void (*OnDeleteInstance) (SkaEntity);
typedef void (*OnStart) (SkaEntity);
typedef void (*OnEnd) (SkaEntity);

typedef void (*OnPreUpdateAll) ();
typedef void (*OnPostUpdateAll) ();
typedef void (*OnUpdateInstance) (SkaEntity, f32);
typedef void (*OnFixedUpdateInstance) (SkaEntity, f32);

typedef void (*OnNetworkCallback) (const char*);

typedef enum CreScriptContextType {
    CreScriptContextType_NONE = -1, // INVALID
    CreScriptContextType_PYTHON = 0,
    CreScriptContextType_NATIVE = 1, // C/C++

    CreScriptContextType_TOTAL_TYPES = 2
} CreScriptContextType;

// Generic script context to be used as an interface for game scripting
// Note: Node event logics must be handled manually within the script context, refer to 'node_event.h' for the api.
// Also check out other script contexts for examples.
typedef struct CREScriptContext {
    // Called when the script context is created
    void (*on_script_context_init)(struct CREScriptContext*);
    // Called when the script context is destroyed
    void (*on_script_context_finalize)(struct CREScriptContext*);
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
    // We could have a validation step on the script contexts to check if the update, fixed_update, etc... funcs exists
    // in the class within the scripting language.  For now, the script context is responsible for entity and entity count
    // even though it's not used in the script ec system
    size_t updateEntityCount;
    size_t fixedUpdateEntityCount;
    SkaEntity updateEntities[SKA_MAX_ENTITIES];
    SkaEntity fixedUpdateEntities[SKA_MAX_ENTITIES];
} CREScriptContext;

typedef void (*OnScriptContextInit) (struct CREScriptContext*);
typedef void (*OnScriptContextFinalize) (struct CREScriptContext*);

typedef struct CREScriptContextTemplate {
    CreScriptContextType contextType;
    OnScriptContextInit on_script_context_init;
    OnScriptContextFinalize on_script_context_finalize;
    OnCreateInstance on_create_instance;
    OnDeleteInstance on_delete_instance;
    OnStart on_start;
    OnPreUpdateAll on_pre_update_all;
    OnPostUpdateAll on_post_update_all;
    OnUpdateInstance on_update_instance;
    OnFixedUpdateInstance on_fixed_update_instance;
    OnEnd on_end;
    OnNetworkCallback on_network_callback;
} CREScriptContextTemplate;

CREScriptContext* cre_script_context_create();
CREScriptContext* cre_script_context_create_from_template(const CREScriptContextTemplate* temp);
void cre_script_context_destroy(CREScriptContext* scriptContext);

#ifdef __cplusplus
}
#endif
