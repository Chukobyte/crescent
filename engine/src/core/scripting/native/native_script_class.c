#include "native_script_class.h"

#include <stddef.h>

#include <seika/memory/se_mem.h>

CRENativeScriptClass* cre_native_class_create_new(SkaEntity entity, const char* path, const char* name) {
    CRENativeScriptClass* scriptClass = SE_MEM_ALLOCATE(CRENativeScriptClass);
    scriptClass->entity = entity;
    scriptClass->path = path;
    scriptClass->name = name;
    scriptClass->create_new_instance_func = NULL;
    scriptClass->on_start_func = NULL;
    scriptClass->on_end_func = NULL;
    scriptClass->update_func = NULL;
    scriptClass->fixed_update_func = NULL;
    scriptClass->instance_data = NULL;
    return scriptClass;
}
