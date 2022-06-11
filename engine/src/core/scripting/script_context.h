#pragma once

#include "../ecs/entity/entity.h"

typedef void (*OnCreateInstance) (Entity, const char*, const char*);
typedef void (*OnDeleteInstance) (Entity);
typedef void (*OnStart) (Entity);
typedef void (*OnUpdateAllInstances) (float);
typedef void (*OnEnd) (Entity);

typedef struct RBEScriptContext {
    OnCreateInstance on_create_instance;
    OnDeleteInstance on_delete_instance;
    OnStart on_start;
    OnUpdateAllInstances on_update_all_instances;
    OnEnd on_end;
} RBEScriptContext;

RBEScriptContext* rbe_script_context_create();
RBEScriptContext* rbe_script_context_get_current();
