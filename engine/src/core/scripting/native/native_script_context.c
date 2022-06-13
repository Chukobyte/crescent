#include "native_script_context.h"

#include "native_script_class.h"
#include "../script_context.h"
#include "../../data_structures/rbe_hash_map_string.h"
#include "../../utils/rbe_assert.h"

// --- Script Context Interface --- //
void native_on_create_instance(Entity entity, const char* classPath, const char* className);
void native_on_delete_instance(Entity entity);
void native_on_start(Entity entity);
void native_on_update_all_instances(float deltaTime);
void native_on_physics_update_all_instances(float deltaTime);
void native_on_end(Entity entity);

// Script Cache
RBEStringHashMap* classCache = NULL;

RBEScriptContext* rbe_native_create_script_context() {
    RBEScriptContext* scriptContext = rbe_script_context_create();
    scriptContext->on_create_instance = native_on_create_instance;
    scriptContext->on_delete_instance = native_on_delete_instance;
    scriptContext->on_start = native_on_start;
    scriptContext->on_update_all_instances = native_on_update_all_instances;
    scriptContext->on_physics_update_all_instances = native_on_physics_update_all_instances;
    scriptContext->on_end = native_on_end;

    RBE_ASSERT(classCache == NULL);
    classCache = rbe_string_hash_map_create(4);

    return scriptContext;
}

void rbe_native_class_register_new_class(RBENativeScriptClass* scriptClass) {
    if (rbe_string_hash_map_has(classCache, scriptClass->name)) {
        rbe_logger_warn("Already have script class registered!\nname: '%s', path: '%s'", scriptClass->name, scriptClass->path);
        return;
    }
    rbe_string_hash_map_add(classCache, scriptClass->name, scriptClass, scriptClass->class_instance_size);
}

void native_on_create_instance(Entity entity, const char* classPath, const char* className) {
    RBE_ASSERT(rbe_string_hash_map_has(classCache, className));
    RBENativeScriptClass* scriptClassRef = rbe_string_hash_map_get(classCache, className);
    RBE_ASSERT(scriptClassRef != NULL);
    RBE_ASSERT(scriptClassRef->create_new_instance_func != NULL);
    rbe_logger_error("native_on_create_instance");
    RBENativeScriptClass* newScriptClass = scriptClassRef->create_new_instance_func();
}

void native_on_delete_instance(Entity entity) {}

void native_on_start(Entity entity) {}

void native_on_update_all_instances(float deltaTime) {}

void native_on_physics_update_all_instances(float deltaTime) {}

void native_on_end(Entity entity) {}
