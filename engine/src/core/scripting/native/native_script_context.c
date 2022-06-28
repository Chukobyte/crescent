#include "native_script_context.h"

#include "../../data_structures/rbe_hash_map.h"
#include "../../data_structures/rbe_hash_map_string.h"
#include "../../data_structures/rbe_static_array.h"
#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"
#include "../script_context.h"

#define MAX_NATIVE_CLASSES        4
#define MAX_NATIVE_CLASS_ENTITIES 8

// --- Script Context Interface --- //
void native_on_create_instance(Entity entity, const char* classPath, const char* className);

void native_on_delete_instance(Entity entity);

void native_on_start(Entity entity);

void native_on_update_all_instances(float deltaTime);

void native_on_physics_update_all_instances(float deltaTime);

void native_on_end(Entity entity);

// Script Cache
RBEStringHashMap* classCache  = NULL;
RBEHashMap* entityToClassName = NULL;

RBE_STATIC_ARRAY_CREATE(RBENativeScriptClass*, MAX_ENTITIES, entities_to_update);
RBE_STATIC_ARRAY_CREATE(RBENativeScriptClass*, MAX_ENTITIES, entities_to_physics_update);

RBEScriptContext* rbe_native_create_script_context() {
    RBEScriptContext* scriptContext                = rbe_script_context_create();
    scriptContext->on_create_instance              = native_on_create_instance;
    scriptContext->on_delete_instance              = native_on_delete_instance;
    scriptContext->on_start                        = native_on_start;
    scriptContext->on_update_all_instances         = native_on_update_all_instances;
    scriptContext->on_physics_update_all_instances = native_on_physics_update_all_instances;
    scriptContext->on_end                          = native_on_end;

    RBE_ASSERT(classCache == NULL);
    classCache = rbe_string_hash_map_create(MAX_NATIVE_CLASSES);

    RBE_ASSERT(entityToClassName == NULL);
    entityToClassName = rbe_hash_map_create(sizeof(Entity), sizeof(RBENativeScriptClass), MAX_NATIVE_CLASS_ENTITIES);

    return scriptContext;
}

void rbe_native_class_register_new_class(RBENativeScriptClass* scriptClass) {
    if (rbe_string_hash_map_has(classCache, scriptClass->name)) {
        rbe_logger_warn("Already have script class registered!\nname: '%s', path: '%s'", scriptClass->name,
                        scriptClass->path);
        return;
    }
    rbe_logger_debug("register native c/c++ class, name: %s, path: %s", scriptClass->name, scriptClass->path);
    rbe_string_hash_map_add(classCache, scriptClass->name, scriptClass, scriptClass->class_instance_size);
}

void native_on_create_instance(Entity entity, const char* classPath, const char* className) {
    RBE_ASSERT_FMT(rbe_string_hash_map_has(classCache, className),
                   "Class ref not cached!  entity: '%d', class_path: '%s', class_name: '%s'", entity, classPath,
                   className);
    RBENativeScriptClass* scriptClassRef = rbe_string_hash_map_get(classCache, className);
    RBE_ASSERT(scriptClassRef != NULL);
    RBE_ASSERT(scriptClassRef->create_new_instance_func != NULL);
    RBENativeScriptClass* newScriptClass = scriptClassRef->create_new_instance_func(entity);
    rbe_hash_map_add(entityToClassName, &entity, newScriptClass);
    if (newScriptClass->update_func != NULL) {
        RBE_STATIC_ARRAY_ADD(entities_to_update, newScriptClass);
    }
    if (newScriptClass->physics_update_func != NULL) {
        RBE_STATIC_ARRAY_ADD(entities_to_physics_update, newScriptClass);
    }
}

void native_on_delete_instance(Entity entity) {
    RBENativeScriptClass* scriptClassRef = (RBENativeScriptClass*) rbe_hash_map_get(entityToClassName, &entity);

    if (scriptClassRef->update_func != NULL) {
        RBE_STATIC_ARRAY_REMOVE(entities_to_update, scriptClassRef, NULL);
    }
    if (scriptClassRef->physics_update_func != NULL) {
        RBE_STATIC_ARRAY_REMOVE(entities_to_physics_update, scriptClassRef, NULL);
    }

    RBE_MEM_FREE(scriptClassRef);
    rbe_hash_map_erase(entityToClassName, &entity);
}

void native_on_start(Entity entity) {
    RBE_ASSERT(rbe_hash_map_has(entityToClassName, &entity));
    RBENativeScriptClass* scriptClassRef = (RBENativeScriptClass*) rbe_hash_map_get(entityToClassName, &entity);
    scriptClassRef->on_start_func(scriptClassRef);
}

void native_on_update_all_instances(float deltaTime) {
    for (size_t i = 0; i < entities_to_update_count; i++) {
        entities_to_update[i]->update_func(entities_to_update[i], deltaTime);
    }
}

void native_on_physics_update_all_instances(float deltaTime) {
    for (size_t i = 0; i < entities_to_physics_update_count; i++) {
        entities_to_physics_update[i]->update_func(entities_to_physics_update[i], deltaTime);
    }
}

void native_on_end(Entity entity) {
    RBE_ASSERT(rbe_hash_map_has(entityToClassName, &entity));
    RBENativeScriptClass* scriptClassRef = (RBENativeScriptClass*) rbe_hash_map_get(entityToClassName, &entity);
    scriptClassRef->on_end_func(scriptClassRef);
}
