#pragma once

#include <stddef.h>

typedef void (*NativeClassOnStartFunc)(void* inst_data);
typedef void (*NativeClassOnEndFunc)(void* inst_data);
typedef void (*NativeClassUpdateFunc)(void* inst_data);
typedef void (*NativeClassPhysicsUpdateFunc)(void* inst_data);

typedef struct RBENativeScriptClass {
    const char* path;
    const char* name;
    struct RBENativeScriptClass* (*create_new_instance_func)();
    NativeClassOnStartFunc on_start_func;
    NativeClassOnEndFunc on_end_func;
    NativeClassUpdateFunc update_func;
    NativeClassPhysicsUpdateFunc physics_update_func;
    void* instance_data;
    size_t class_instance_size;
} RBENativeScriptClass;

RBENativeScriptClass* rbe_native_class_create_new(const char* path, const char* name);
