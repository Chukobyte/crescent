#include "cre_pkpy_script_context.h"
/*
#include <string.h>

#include <pocketpy/pocketpy_c.h>

#include <seika/memory.h>
#include <seika/assert.h>
#include <seika/string.h>
#include <seika/logger.h>
#include <seika/networking/network.h>
#include <seika/data_structures/static_array.h>
#include <seika/asset/asset_file_loader.h>

#include "cre_pkpy.h"
#include "cre_pkpy_entity_instance_cache.h"
#include "cre_pkpy_node_event.h"
#include "cre_pkpy_node_event_manager.h"
#include "api/cre_pkpy_api.h"
#include "../../script_context.h"
#include "../../../ecs/components/animated_sprite_component.h"
#include "../../../ecs/component.h"
#include "../../../ecs/components/node_component.h"
#include "../../../ecs/components/script_component.h"
#include "../../../engine_context.h"
#include "../../../ecs/ecs_globals.h"

//--- Script Context Interface ---//
void pkpy_sc_on_delete_instance(SkaEntity entity);
void pkpy_sc_on_start(SkaEntity entity);
void pkpy_sc_on_update_instance(SkaEntity entity, f32 deltaTime);
void pkpy_sc_on_fixed_update_instance(SkaEntity entity, f32 deltaTime);
void pkpy_sc_on_end(SkaEntity entity);
void pkpy_sc_on_network_callback(const char* message);
void pkpy_sc_on_script_context_destroy();

// Custom Import Handler
unsigned char*  cre_pkpy_import_handler(const char* path, int32 pathSize, int32* outSize);

//--- Node Event Callbacks ---//
void pkpy_node_event_node_scene_entered(SkaSubjectNotifyPayload* payload);
void pkpy_node_event_node_scene_exited(SkaSubjectNotifyPayload* payload);

void pkpy_node_event_animated_sprite_frame_changed(SkaSubjectNotifyPayload* payload);
void pkpy_node_event_animated_sprite_animation_finished(SkaSubjectNotifyPayload* payload);

// Observers
SkaObserver node_scene_entered_observer = { .on_notify = pkpy_node_event_node_scene_entered };
SkaObserver node_scene_exited_observer = { .on_notify = pkpy_node_event_node_scene_exited };

SkaObserver animated_sprite_frame_changed_observer = { .on_notify = pkpy_node_event_animated_sprite_frame_changed };
SkaObserver animated_sprite_animation_finished_observer = { .on_notify = pkpy_node_event_animated_sprite_animation_finished };

CREScriptContext* pkpy_script_context = NULL;
pkpy_vm* cre_pkpy_vm = NULL;

pkpy_CName pyStartFunctionName;
pkpy_CName pyProcessFunctionName;
pkpy_CName pyFixedProcessFunctionName;
pkpy_CName pyEndFunctionName;

SKA_STATIC_ARRAY_CREATE(SkaEntity, SKA_MAX_ENTITIES, entityInitializedList);

CREScriptContext* cre_pkpy_script_context_create() {
    SKA_ASSERT_FMT(pkpy_script_context == NULL, "Script context already created!");
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = cre_pkpy_script_context_create_instance_if_nonexistent; // Using nonexistent version in case added to entity cache from elsewhere
    scriptContext->on_delete_instance = pkpy_sc_on_delete_instance;
    scriptContext->on_start = pkpy_sc_on_start;
    scriptContext->on_update_instance = pkpy_sc_on_update_instance;
    scriptContext->on_fixed_update_instance = pkpy_sc_on_fixed_update_instance;
    scriptContext->on_end = pkpy_sc_on_end;
    scriptContext->on_network_callback = pkpy_sc_on_network_callback;
    scriptContext->on_script_context_destroy = pkpy_sc_on_script_context_destroy;

    pkpy_script_context = scriptContext;

    if (!cre_pkpy_vm) {
        cre_pkpy_vm = pkpy_new_vm(false);
        pyStartFunctionName = pkpy_name("_start");
        pyProcessFunctionName = pkpy_name("_process");
        pyFixedProcessFunctionName = pkpy_name("_fixed_process");
        pyEndFunctionName = pkpy_name("_end");

        pkpy_set_import_handler(cre_pkpy_vm, cre_pkpy_import_handler);
        // TODO: create and set pkpy output handler
    }

    cre_pkpy_api_load_internal_modules(cre_pkpy_vm);
    cre_pkpy_node_event_manager_initialize(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_initialize(cre_pkpy_vm);

    return scriptContext;
}

CREScriptContext* cre_pkpy_script_context_get() {
    return pkpy_script_context;
}

void cre_pkpy_script_context_setup_node_event(SkaEntity entity) {
    if (!entityInitializedList[entity]) {
        entityInitializedList[entity] = true;
        // Node
        NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
        SKA_ASSERT_FMT(nodeComponent, "Node component should never be null!  Entity id: '%u'", entity);
        ska_event_register_observer(&nodeComponent->onSceneTreeEnter, &node_scene_entered_observer);
        ska_event_register_observer(&nodeComponent->onSceneTreeExit, &node_scene_exited_observer);
        // Animated Sprite
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) ska_ecs_component_manager_get_component_unchecked(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
        if (animatedSpriteComponent) {
            ska_event_register_observer(&animatedSpriteComponent->onFrameChanged, &animated_sprite_frame_changed_observer);
            ska_event_register_observer(&animatedSpriteComponent->onAnimationFinished, &animated_sprite_animation_finished_observer);
        }
    }
}

void cre_pkpy_script_context_create_instance(SkaEntity entity, const char* classPath, const char* className) {
    SKA_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_create_new_entity(cre_pkpy_vm, classPath, className, entity);
    cre_pkpy_script_context_setup_node_event(entity);
}

void cre_pkpy_script_context_create_instance_if_nonexistent(SkaEntity entity, const char* classPath, const char* className) {
    SKA_ASSERT(cre_pkpy_vm);
    // If exists, just make sure node events are setup
    if (cre_pkpy_entity_instance_cache_has_entity(cre_pkpy_vm, entity)) {
        cre_pkpy_script_context_setup_node_event(entity);
    } else {
        cre_pkpy_script_context_create_instance(entity, classPath, className);
    }
}

void cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(SkaEntity entity) {
    SKA_ASSERT(cre_pkpy_vm);
    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component_unchecked(entity, SCRIPT_COMPONENT_INDEX);
    if (!scriptComponent) {
        // If an entity doesn't have a script component at this point, it should just be a plain base class from the crescent module.
        // Since that is the case, just create a script component so that we can clean up within the script context when the entity
        // needs to leave the instance cache.
        const char* baseClassName = node_get_base_type_string(nodeComponent->type);
        scriptComponent = script_component_create_ex(CRE_PKPY_MODULE_NAME_CRESCENT, baseClassName, ScriptContextType_PYTHON);
        ska_ecs_component_manager_set_component(entity, SCRIPT_COMPONENT_INDEX, scriptComponent);
    }
    cre_pkpy_script_context_create_instance_if_nonexistent(entity, scriptComponent->classPath, scriptComponent->className);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
}

void pkpy_sc_on_delete_instance(SkaEntity entity) {
    SKA_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_remove_entity(cre_pkpy_vm, entity);

    ska_array_utils_remove_item_uint32(
            pkpy_script_context->updateEntities,
            &pkpy_script_context->updateEntityCount,
            entity,
            SKA_NULL_ENTITY
    );

    ska_array_utils_remove_item_uint32(
            pkpy_script_context->fixedUpdateEntities,
            &pkpy_script_context->fixedUpdateEntityCount,
            entity,
            SKA_NULL_ENTITY
    );

    entityInitializedList[entity] = false;
}

void pkpy_sc_on_start(SkaEntity entity) {
    SKA_ASSERT(cre_pkpy_vm);
    // Check for process funcs
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyStartFunctionName)) {
        pkpy_push_null(cre_pkpy_vm);
        pkpy_vectorcall(cre_pkpy_vm, 0);
    }
    pkpy_pop_top(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyProcessFunctionName)) {
        pkpy_script_context->updateEntities[pkpy_script_context->updateEntityCount++] = entity;
    }
    pkpy_pop_top(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyFixedProcessFunctionName)) {
        pkpy_script_context->fixedUpdateEntities[pkpy_script_context->fixedUpdateEntityCount++] = entity;
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_update_instance(SkaEntity entity, float deltaTime) {
    SKA_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    const bool hasFunc = pkpy_getattr(cre_pkpy_vm, pyProcessFunctionName);
    SKA_ASSERT(hasFunc);
    pkpy_push_null(cre_pkpy_vm);
    pkpy_push_float(cre_pkpy_vm, (f64)deltaTime);
    pkpy_vectorcall(cre_pkpy_vm, 1);
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_fixed_update_instance(SkaEntity entity, float deltaTime) {
    SKA_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    const bool hasFunc = pkpy_getattr(cre_pkpy_vm, pyFixedProcessFunctionName);
    SKA_ASSERT(hasFunc);
    pkpy_push_null(cre_pkpy_vm);
    pkpy_push_float(cre_pkpy_vm, (f64)deltaTime);
    pkpy_vectorcall(cre_pkpy_vm, 1);
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_end(SkaEntity entity) {
    SKA_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyEndFunctionName)) {
        pkpy_push_null(cre_pkpy_vm);
        pkpy_vectorcall(cre_pkpy_vm, 0);
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_network_callback(const char* message) {
    SKA_ASSERT(cre_pkpy_vm);
    cre_pkpy_node_event_manager_broadcast_event_string(cre_pkpy_vm, CRE_PKPY_NODE_EVENT_OWNER_ID_NETWORK, CRE_PKPY_NODE_EVENT_NAME_MESSAGE_RECEIVED, message);
}

void pkpy_sc_on_script_context_destroy() {
    cre_pkpy_node_event_manager_finalize();
    cre_pkpy_entity_instance_cache_finalize(cre_pkpy_vm);

    SKA_ASSERT(cre_pkpy_vm);
    // TODO: Known memory leak here due to issue with deleting pkpy vm multiple times during testing, not an issue in real world since we won't re-init the vm
    // pkpy_delete_vm(cre_pkpy_vm);
    cre_pkpy_vm = NULL;

    SKA_MEM_FREE(pkpy_script_context);
    pkpy_script_context = NULL;

    memset(entityInitializedList, 0, sizeof(entityInitializedList));
}

pkpy_vm* cre_pkpy_script_context_get_active_pkpy_vm() {
    return cre_pkpy_vm;
}

void cre_pkpy_script_context_on_network_udp_server_client_connected() {}

//--- Custom Import Handler ---//
unsigned char* cre_pkpy_import_handler(const char* path, int pathSize, int* outSize) {
#define CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE 512
    SKA_ASSERT_FMT(
            pathSize <= CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE,
            "Passed in pkpy path size is '%d' while 'CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE' is '%d', consider increasing CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE!",
            pathSize, CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE
    );
    // Construct path, adds full path if loading from disk
    char pathBuffer[CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE];
    ska_str_trim_by_size(path, pathBuffer, pathSize);
    ska_logger_debug("Importing pkpy module from path '%s'", pathBuffer);
    // Now attempt to load
    char* moduleString = ska_asset_file_loader_read_file_contents_as_string(pathBuffer, NULL);
    if (!moduleString) {
        ska_logger_error("Failed to load pkpy module at path'%s'", pathBuffer);
        *outSize = 0;
        return NULL;
    }
    size_t moduleDataSize;
    unsigned char* cachedImportData = ska_str_convert_string_to_unsigned_char(moduleString, &moduleDataSize);
    SKA_MEM_FREE(moduleString);
    *outSize = (int)moduleDataSize;
    return cachedImportData;
#undef CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE
}

//--- Node event callbacks ---//

// NODE
void pkpy_node_event_node_scene_entered(SkaSubjectNotifyPayload* payload) {
    const SkaEntity entity = *(SkaEntity*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event(cre_pkpy_vm, (int)entity, CRE_PKPY_NODE_EVENT_NAME_SCENE_ENTERED);
}

void pkpy_node_event_node_scene_exited(SkaSubjectNotifyPayload* payload) {
    const SkaEntity entity = *(SkaEntity*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event(cre_pkpy_vm, (int)entity, CRE_PKPY_NODE_EVENT_NAME_SCENE_EXITED);
    cre_pkpy_node_event_manager_remove_entity_and_connections(cre_pkpy_vm, (int)entity);
}

// ANIMATED SPRITE
void pkpy_node_event_animated_sprite_frame_changed(SkaSubjectNotifyPayload* payload) {
    const AnimatedSpriteFrameChangedPayload* eventPayload = (AnimatedSpriteFrameChangedPayload*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event_int(cre_pkpy_vm, (int)eventPayload->entity, CRE_PKPY_NODE_EVENT_NAME_FRAME_CHANGED, eventPayload->newFrame);
}

void pkpy_node_event_animated_sprite_animation_finished(SkaSubjectNotifyPayload* payload) {
    const AnimatedSpriteAnimationFinishedPayload* eventPayload = (AnimatedSpriteAnimationFinishedPayload*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event_string(cre_pkpy_vm, (int)eventPayload->entity, CRE_PKPY_NODE_EVENT_NAME_ANIMATION_FINISHED, eventPayload->animation->name);
}
*/
