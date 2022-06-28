#pragma once

#include <stddef.h>

#include "../../ecs/entity/entity.h"

typedef struct RBENativeScriptClass {
    const char* path;
    const char* name;
    Entity entity;

    struct RBENativeScriptClass* (*create_new_instance_func)(Entity);

    void (*on_start_func)(struct RBENativeScriptClass* nativeScriptClass);

    void (*on_end_func)(struct RBENativeScriptClass* nativeScriptClass);

    void (*update_func)(struct RBENativeScriptClass* nativeScriptClass, float deltaTime);

    void (*physics_update_func)(struct RBENativeScriptClass* nativeScriptClass, float deltaTime);

    void* instance_data;
    size_t class_instance_size;
} RBENativeScriptClass;

RBENativeScriptClass* rbe_native_class_create_new(Entity entity, const char* path, const char* name);
