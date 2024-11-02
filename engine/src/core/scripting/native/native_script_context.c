#include "native_script_context.h"

#include <seika/assert.h>
#include <seika/memory.h>
#include <seika/logger.h>
#include <seika/data_structures/hash_map.h>
#include <seika/data_structures/hash_map_string.h>
#include <seika/data_structures/static_array.h>

#include "native_script_class.h"
#include "internal_classes/fps_display_class.h"

#define MAX_NATIVE_CLASSES 4
#define MAX_NATIVE_CLASS_ENTITIES 8

// --- Script Context Interface --- //
static void native_init(CREScriptContext* scriptContext);
static void native_finalize(CREScriptContext* scriptContext);
static void native_on_create_instance(SkaEntity entity, const char* classPath, const char* className);
static void native_on_delete_instance(SkaEntity entity);
static void native_on_start(SkaEntity entity);
static void native_on_update_instance(SkaEntity entity, f32 deltaTime);
static void native_on_fixed_update_instance(SkaEntity entity, f32 deltaTime);
static void native_on_end(SkaEntity entity);

// Script Cache
SkaStringHashMap* classCache = NULL;
SkaHashMap* entityToClassName = NULL;

CREScriptContext* native_script_context = NULL;

CREScriptContextTemplate cre_native_get_script_context_template() {
    return (CREScriptContextTemplate) {
        .contextType = CreScriptContextType_NATIVE,
        .on_script_context_init = native_init,
        .on_script_context_finalize = native_finalize,
        .on_create_instance = native_on_create_instance,
        .on_delete_instance = native_on_delete_instance,
        .on_start = native_on_start,
        .on_pre_update_all = NULL,
        .on_post_update_all = NULL,
        .on_update_instance = native_on_update_instance,
        .on_fixed_update_instance = native_on_fixed_update_instance,
        .on_end = native_on_end,
        .on_network_callback = NULL,
    };
}

void native_init(CREScriptContext* context) {
    SKA_ASSERT(classCache == NULL);
    classCache = ska_string_hash_map_create(MAX_NATIVE_CLASSES);

    SKA_ASSERT(entityToClassName == NULL);
    entityToClassName = ska_hash_map_create(sizeof(SkaEntity), sizeof(CRENativeScriptClass **), MAX_NATIVE_CLASS_ENTITIES);

    // Register internal classes
    cre_native_class_register_new_class(fps_display_native_class_create_new());

    native_script_context = context;
}

void native_finalize(CREScriptContext* context) {
    SKA_ASSERT(classCache != NULL);
    SKA_ASSERT(entityToClassName != NULL);
    SKA_ASSERT(native_script_context != NULL);

    native_script_context = NULL;

    ska_string_hash_map_destroy(classCache);
    ska_hash_map_destroy(entityToClassName);
    classCache = NULL;
    entityToClassName = NULL;
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

    SKA_FREE(scriptClassRef);
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
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) ska_hash_map_get(entityToClassName, &entity);
    scriptClassRef->on_end_func(scriptClassRef);
}
