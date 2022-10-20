#include "native_script_class.h"

#include <stddef.h>

#include "../seika/src/memory/rbe_mem.h"

RBENativeScriptClass* rbe_native_class_create_new(Entity entity, const char* path, const char* name) {
    RBENativeScriptClass* scriptClass = RBE_MEM_ALLOCATE(RBENativeScriptClass);
    scriptClass->entity = entity;
    scriptClass->path = path;
    scriptClass->name = name;
    scriptClass->create_new_instance_func = NULL;
    scriptClass->on_start_func = NULL;
    scriptClass->on_end_func = NULL;
    scriptClass->update_func = NULL;
    scriptClass->physics_update_func = NULL;
    scriptClass->instance_data = NULL;
    return scriptClass;
}
