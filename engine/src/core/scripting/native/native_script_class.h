#pragma once

#include <stddef.h>

#include "../../ecs/entity/entity.h"

typedef void (*NativeClassOnStartFunc)(Entity entity, void* inst_data);
typedef void (*NativeClassOnEndFunc)(Entity entity, void* inst_data);
typedef void (*NativeClassUpdateFunc)(Entity entity, void* inst_data, float deltaTime);
typedef void (*NativeClassPhysicsUpdateFunc)(Entity entity, void* inst_data, float deltaTime);

typedef struct RBENativeScriptClass {
    const char* path;
    const char* name;
    Entity entity;
    struct RBENativeScriptClass* (*create_new_instance_func)(Entity);
    NativeClassOnStartFunc on_start_func;
    NativeClassOnEndFunc on_end_func;
    NativeClassUpdateFunc update_func;
    NativeClassPhysicsUpdateFunc physics_update_func;
    void* instance_data;
    size_t class_instance_size;
} RBENativeScriptClass;

RBENativeScriptClass* rbe_native_class_create_new(Entity entity, const char* path, const char* name);
