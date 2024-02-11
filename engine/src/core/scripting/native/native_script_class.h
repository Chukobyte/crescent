#pragma once

#include <stddef.h>

#include <seika/ecs/entity.h>

typedef struct CRENativeScriptClass {
    const char* path;
    const char* name;
    SkaEntity entity;
    struct CRENativeScriptClass* (*create_new_instance_func)(SkaEntity);
    void (*on_start_func)(struct CRENativeScriptClass* nativeScriptClass);
    void (*on_end_func)(struct CRENativeScriptClass* nativeScriptClass);
    void (*update_func)(struct CRENativeScriptClass* nativeScriptClass, float deltaTime);
    void (*fixed_update_func)(struct CRENativeScriptClass* nativeScriptClass, float deltaTime);
    void* instance_data;
    size_t class_instance_size;
} CRENativeScriptClass;

CRENativeScriptClass* cre_native_class_create_new(SkaEntity entity, const char* path, const char* name);
