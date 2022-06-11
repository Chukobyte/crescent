#pragma once

#include "../ecs/entity/entity.h"

typedef void (*OnCreateInstance) (Entity, const char*, const char*);
typedef void (*OnDeleteInstance) (Entity);
typedef void (*OnStart) (Entity);
typedef void (*OnUpdate) (Entity, float);
typedef void (*OnEnd) (Entity);

typedef struct RBEScriptContext {
    OnCreateInstance on_create_instance;
    OnDeleteInstance on_delete_instance;
    OnStart on_start;
    OnUpdate on_update;
    OnEnd on_end;
} RBEScriptContext;

RBEScriptContext* rbe_script_context_create();
