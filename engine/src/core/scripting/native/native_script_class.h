#pragma once

#include <stddef.h>

#include "../../ecs/entity/entity.h"

typedef struct CRENativeScriptClass {
    const char* path;
    const char* name;
    CreEntity entity;
    struct CRENativeScriptClass* (*create_new_instance_func)(CreEntity);
    void (*on_start_func)(struct CRENativeScriptClass* nativeScriptClass);
    void (*on_end_func)(struct CRENativeScriptClass* nativeScriptClass);
    void (*update_func)(struct CRENativeScriptClass* nativeScriptClass, float deltaTime);
    void (*fixed_update_func)(struct CRENativeScriptClass* nativeScriptClass, float deltaTime);
    void* instance_data;
    size_t class_instance_size;
} CRENativeScriptClass;

CRENativeScriptClass* cre_native_class_create_new(CreEntity entity, const char* path, const char* name);
