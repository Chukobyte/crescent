#pragma once

#include "../ecs/entity/entity.h"

typedef void (*OnCreateInstance) (Entity, const char*, const char*);
typedef void (*OnDeleteInstance) (Entity);
typedef void (*OnStart) (Entity);
typedef void (*OnUpdateAllInstances) (float);
typedef void (*OnPhysicsUpdateAllInstances) (float);
typedef void (*OnEnd) (Entity);
typedef void (*OnNetworkCallback) (const char*);

typedef struct RBEScriptContext {
    OnCreateInstance on_create_instance;
    OnDeleteInstance on_delete_instance;
    OnStart on_start;
    OnUpdateAllInstances on_update_all_instances;
    OnPhysicsUpdateAllInstances on_physics_update_all_instances;
    OnEnd on_end;
    OnNetworkCallback on_network_callback;
} RBEScriptContext;

RBEScriptContext* rbe_script_context_create();
