#include "native_script_context.h"

#include <seika/assert.h>
#include <seika/memory.h>
#include <seika/logger.h>
#include <seika/data_structures/hash_map.h>
#include <seika/data_structures/hash_map_string.h>
#include <seika/data_structures/static_array.h>

#include "native_script_class.h"
#include "../script_context.h"

#define MAX_NATIVE_CLASSES 4
#define MAX_NATIVE_CLASS_ENTITIES 8

// --- Script Context Interface --- //
void native_on_create_instance(SkaEntity entity, const char* classPath, const char* className);
void native_on_delete_instance(SkaEntity entity);
void native_on_start(SkaEntity entity);
void native_on_update_instance(SkaEntity entity, f32 deltaTime);
void native_on_fixed_update_instance(SkaEntity entity, f32 deltaTime);
void native_on_end(SkaEntity entity);
void native_on_script_context_destroy();

// Script Cache
SkaStringHashMap* classCache = NULL;
SkaHashMap* entityToClassName = NULL;

CREScriptContext* native_script_context = NULL;

CREScriptContext* cre_native_create_script_context() {
    SKA_ASSERT(native_script_context == NULL);
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = native_on_create_instance;
    scriptContext->on_delete_instance = native_on_delete_instance;
    scriptContext->on_start = native_on_start;
    scriptContext->on_update_instance = native_on_update_instance;
    scriptContext->on_fixed_update_instance = native_on_fixed_update_instance;
    scriptContext->on_end = native_on_end;
    scriptContext->on_script_context_destroy = native_on_script_context_destroy;

    SKA_ASSERT(classCache == NULL);
    classCache = ska_string_hash_map_create(MAX_NATIVE_CLASSES);

    SKA_ASSERT(entityToClassName == NULL);
    entityToClassName = ska_hash_map_create(sizeof(SkaEntity), sizeof(CRENativeScriptClass **), MAX_NATIVE_CLASS_ENTITIES);

    native_script_context = scriptContext;

    return scriptContext;
}

void cre_native_class_register_new_class(CRENativeScriptClass* scriptClass) {
    if (ska_string_hash_map_has(classCache, scriptClass->name)) {
        ska_logger_warn("Already have script class registered!\nname: '%s', path: '%s'", scriptClass->name,
                       scriptClass->path);
        return;
    }
    ska_logger_debug("register native c/c++ class, name: %s, path: %s", scriptClass->name, scriptClass->path);
    ska_string_hash_map_add(classCache, scriptClass->name, &scriptClass, scriptClass->class_instance_size);
}

void native_on_create_instance(SkaEntity entity, const char* classPath, const char* className) {
    SKA_ASSERT_FMT(ska_string_hash_map_has(classCache, className), "Class ref not cached!  entity: '%d', class_path: '%s', class_name: '%s'", entity, classPath, className);
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) ska_string_hash_map_get(
            classCache, className);
    SKA_ASSERT(scriptClassRef != NULL);
    SKA_ASSERT(scriptClassRef->create_new_instance_func != NULL);
    CRENativeScriptClass* newScriptClass = scriptClassRef->create_new_instance_func(entity);
    ska_hash_map_add(entityToClassName, &entity, &newScriptClass);
}

void native_on_delete_instance(SkaEntity entity) {
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**)ska_hash_map_get(entityToClassName, &entity);

    if (scriptClassRef->update_func != NULL) {
        ska_array_utils_remove_item_uint32(
            native_script_context->updateEntities,
            &native_script_context->updateEntityCount,
            entity,
            SKA_NULL_ENTITY
        );
    }
    if (scriptClassRef->fixed_update_func != NULL) {
        ska_array_utils_remove_item_uint32(
            native_script_context->fixedUpdateEntities,
            &native_script_context->fixedUpdateEntityCount,
            entity,
            SKA_NULL_ENTITY
        );
    }

    SKA_MEM_FREE(scriptClassRef);
    ska_hash_map_erase(entityToClassName, &entity);
}

void native_on_start(SkaEntity entity) {
    SKA_ASSERT(ska_hash_map_has(entityToClassName, &entity));
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) ska_hash_map_get(entityToClassName, &entity);
    scriptClassRef->on_start_func(scriptClassRef);
    // Check if entity has update functions
    if (scriptClassRef->update_func != NULL) {
        native_script_context->updateEntities[native_script_context->updateEntityCount++] = entity;
    }
    if (scriptClassRef->fixed_update_func != NULL) {
        native_script_context->fixedUpdateEntities[native_script_context->fixedUpdateEntityCount++] = entity;
    }
}

void native_on_update_instance(SkaEntity entity, f32 deltaTime) {
    CRENativeScriptClass* scriptClass = (CRENativeScriptClass*) *(CRENativeScriptClass**) ska_hash_map_get(entityToClassName, &entity);
    scriptClass->update_func(scriptClass, deltaTime);
}

void native_on_fixed_update_instance(SkaEntity entity, float deltaTime) {
    CRENativeScriptClass* scriptClass = (CRENativeScriptClass*) *(CRENativeScriptClass**) ska_hash_map_get(entityToClassName, &entity);
    scriptClass->fixed_update_func(scriptClass, deltaTime);
}

void native_on_end(SkaEntity entity) {
    SKA_ASSERT(ska_hash_map_has(entityToClassName, &entity));
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) ska_hash_map_get(
            entityToClassName, &entity);
    scriptClassRef->on_end_func(scriptClassRef);
}

void native_on_script_context_destroy() {
    native_script_context = NULL;

    ska_string_hash_map_destroy(classCache);
    ska_hash_map_destroy(entityToClassName);
    classCache = NULL;
    entityToClassName = NULL;
}
