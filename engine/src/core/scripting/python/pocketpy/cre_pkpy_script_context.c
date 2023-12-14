#include "cre_pkpy_script_context.h"

#include <string.h>

#include <pocketpy/pocketpy_c.h>

#include <seika/networking/se_network.h>
#include <seika/data_structures/se_static_array.h>
#include <seika/asset/asset_file_loader.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_assert.h>
#include <seika/utils/se_string_util.h>

#include "cre_pkpy.h"
#include "cre_pkpy_entity_instance_cache.h"
#include "cre_pkpy_node_event.h"
#include "cre_pkpy_node_event_manager.h"
#include "api/cre_pkpy_api.h"
#include "../../script_context.h"
#include "../../../ecs/component/animated_sprite_component.h"
#include "../../../ecs/component/component.h"
#include "../../../ecs/component/node_component.h"
#include "../../../ecs/component/script_component.h"
#include "../../../engine_context.h"

//--- Script Context Interface ---//
void pkpy_sc_on_delete_instance(CreEntity entity);
void pkpy_sc_on_start(CreEntity entity);
void pkpy_sc_on_update_instance(CreEntity entity, float deltaTime);
void pkpy_sc_on_fixed_update_instance(CreEntity entity, float deltaTime);
void pkpy_sc_on_end(CreEntity entity);
void pkpy_sc_on_network_callback(const char* message);
void pkpy_sc_on_script_context_destroy();

// Custom Import Handler
unsigned char*  cre_pkpy_import_handler(const char* path, int pathSize, int* outSize);

//--- Node Event Callbacks ---//
void pkpy_node_event_node_scene_entered(SESubjectNotifyPayload* payload);
void pkpy_node_event_node_scene_exited(SESubjectNotifyPayload* payload);

void pkpy_node_event_animated_sprite_frame_changed(SESubjectNotifyPayload* payload);
void pkpy_node_event_animated_sprite_animation_finished(SESubjectNotifyPayload* payload);

// Observers
SEObserver node_scene_entered_observer = { .on_notify = pkpy_node_event_node_scene_entered };
SEObserver node_scene_exited_observer = { .on_notify = pkpy_node_event_node_scene_exited };

SEObserver animated_sprite_frame_changed_observer = { .on_notify = pkpy_node_event_animated_sprite_frame_changed };
SEObserver animated_sprite_animation_finished_observer = { .on_notify = pkpy_node_event_animated_sprite_animation_finished };

CREScriptContext* pkpy_script_context = NULL;
pkpy_vm* cre_pkpy_vm = NULL;

pkpy_CName pyStartFunctionName;
pkpy_CName pyUpdateFunctionName;
pkpy_CName pyFixedUpdateFunctionName;
pkpy_CName pyEndFunctionName;

SE_STATIC_ARRAY_CREATE(CreEntity, CRE_MAX_ENTITIES, entityInitializedList);

CREScriptContext* cre_pkpy_script_context_create() {
    SE_ASSERT_FMT(pkpy_script_context == NULL, "Script context already created!");
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
        pyUpdateFunctionName = pkpy_name("_process");
        pyFixedUpdateFunctionName = pkpy_name("_fixed_process");
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

void cre_pkpy_script_context_setup_node_event(CreEntity entity) {
    if (!entityInitializedList[entity]) {
        entityInitializedList[entity] = true;
        // Node
        NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_NODE);
        SE_ASSERT_FMT(nodeComponent, "Node component should never be null!  Node name: '%s', entity id: '%u'", nodeComponent->name, entity);
        se_event_register_observer(&nodeComponent->onSceneTreeEnter, &node_scene_entered_observer);
        se_event_register_observer(&nodeComponent->onSceneTreeExit, &node_scene_exited_observer);
        // Animated Sprite
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_ANIMATED_SPRITE);
        if (animatedSpriteComponent) {
            se_event_register_observer(&animatedSpriteComponent->onFrameChanged, &animated_sprite_frame_changed_observer);
            se_event_register_observer(&animatedSpriteComponent->onAnimationFinished, &animated_sprite_animation_finished_observer);
        }
    }
}

void cre_pkpy_script_context_create_instance(CreEntity entity, const char* classPath, const char* className) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_create_new_entity(cre_pkpy_vm, classPath, className, entity);
    cre_pkpy_script_context_setup_node_event(entity);
    // Check funcs
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyUpdateFunctionName)) {
        pkpy_script_context->updateEntities[pkpy_script_context->updateEntityCount++] = entity;
    }
    if (pkpy_getattr(cre_pkpy_vm, pyFixedUpdateFunctionName)) {
        pkpy_script_context->fixedUpdateEntities[pkpy_script_context->fixedUpdateEntityCount++] = entity;
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void cre_pkpy_script_context_create_instance_if_nonexistent(CreEntity entity, const char* classPath, const char* className) {
    SE_ASSERT(cre_pkpy_vm);
    // If exists, just make sure node events are setup
    if (cre_pkpy_entity_instance_cache_has_entity(cre_pkpy_vm, entity)) {
        cre_pkpy_script_context_setup_node_event(entity);
    } else {
        cre_pkpy_script_context_create_instance(entity, classPath, className);
    }
}

void cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(CreEntity entity) {
    SE_ASSERT(cre_pkpy_vm);
    const NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_NODE);
    ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_SCRIPT);
    if (!scriptComponent) {
        // If an entity doesn't have a script component at this point, it should just be a plain base class from the crescent module.
        // Since that is the case, just create a script component so that we can clean up within the script context when the entity
        // needs to leave the instance cache.
        const char* baseClassName = node_get_base_type_string(nodeComponent->type);
        scriptComponent = script_component_create_ex(CRE_PKPY_MODULE_NAME_CRESCENT, baseClassName, ScriptContextType_PYTHON);
        cre_component_manager_set_component(entity, CreComponentDataIndex_SCRIPT, scriptComponent);
    }
    cre_pkpy_script_context_create_instance_if_nonexistent(entity, scriptComponent->classPath, scriptComponent->className);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
}

void pkpy_sc_on_delete_instance(CreEntity entity) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_remove_entity(cre_pkpy_vm, entity);

    se_array_utils_remove_item_uint32(
            pkpy_script_context->updateEntities,
            &pkpy_script_context->updateEntityCount,
            entity,
            CRE_NULL_ENTITY
    );

    se_array_utils_remove_item_uint32(
            pkpy_script_context->fixedUpdateEntities,
            &pkpy_script_context->fixedUpdateEntityCount,
            entity,
            CRE_NULL_ENTITY
    );

    entityInitializedList[entity] = false;
}

void pkpy_sc_on_start(CreEntity entity) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyStartFunctionName)) {
        pkpy_push_null(cre_pkpy_vm);
        pkpy_vectorcall(cre_pkpy_vm, 0);
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_update_instance(CreEntity entity, float deltaTime) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyUpdateFunctionName)) {
        pkpy_push_null(cre_pkpy_vm);
        pkpy_push_float(cre_pkpy_vm, (double)deltaTime);
        pkpy_vectorcall(cre_pkpy_vm, 1);
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_fixed_update_instance(CreEntity entity, float deltaTime) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyFixedUpdateFunctionName)) {
        pkpy_push_null(cre_pkpy_vm);
        pkpy_push_float(cre_pkpy_vm, (double)deltaTime);
        pkpy_vectorcall(cre_pkpy_vm, 1);
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_end(CreEntity entity) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(cre_pkpy_vm, entity);
    if (pkpy_getattr(cre_pkpy_vm, pyEndFunctionName)) {
        pkpy_push_null(cre_pkpy_vm);
        pkpy_vectorcall(cre_pkpy_vm, 0);
    }
    pkpy_pop_top(cre_pkpy_vm);
}

void pkpy_sc_on_network_callback(const char* message) {
    SE_ASSERT(cre_pkpy_vm);
    cre_pkpy_node_event_manager_broadcast_event_string(cre_pkpy_vm, CRE_PKPY_NODE_EVENT_OWNER_ID_NETWORK, CRE_PKPY_NODE_EVENT_NAME_MESSAGE_RECEIVED, message);
}

void pkpy_sc_on_script_context_destroy() {
    cre_pkpy_node_event_manager_finalize();
    cre_pkpy_entity_instance_cache_finalize(cre_pkpy_vm);

    SE_ASSERT(cre_pkpy_vm);
    pkpy_delete_vm(cre_pkpy_vm);
    cre_pkpy_vm = NULL;

    SE_MEM_FREE(pkpy_script_context);
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
    SE_ASSERT_FMT(
            pathSize <= CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE,
            "Passed in pkpy path size is '%d' while 'CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE' is '%d', consider increasing CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE!",
            pathSize, CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE
    );
    // Construct path, adds full path if loading from disk
    char pathBuffer[CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE];
    se_str_trim_by_size(path, pathBuffer, pathSize);
    se_logger_debug("Importing pkpy module from path '%s'", pathBuffer);
    // Now attempt to load
    size_t moduleStringSize;
    char* moduleString = sf_asset_file_loader_read_file_contents_as_string(pathBuffer, &moduleStringSize);
    if (!moduleString) {
        se_logger_error("Failed to load pkpy module at path'%s'", pathBuffer);
        *outSize = 0;
        return NULL;
    }
    unsigned char* cachedImportData = se_str_convert_string_to_unsigned_char(moduleString, &moduleStringSize);
    SE_MEM_FREE(moduleString);
    *outSize = (int)moduleStringSize;
    return cachedImportData;
#undef CRE_PKPY_IMPORT_HANDLER_PATH_BUFFER_SIZE
}

//--- Node event callbacks ---//

// NODE
void pkpy_node_event_node_scene_entered(SESubjectNotifyPayload* payload) {
    const CreEntity entity = *(CreEntity*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event(cre_pkpy_vm, (int)entity, CRE_PKPY_NODE_EVENT_NAME_SCENE_ENTERED);
}

void pkpy_node_event_node_scene_exited(SESubjectNotifyPayload* payload) {
    const CreEntity entity = *(CreEntity*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event(cre_pkpy_vm, (int)entity, CRE_PKPY_NODE_EVENT_NAME_SCENE_EXITED);
    cre_pkpy_node_event_manager_remove_entity_and_connections(cre_pkpy_vm, (int)entity);
}

// ANIMATED SPRITE
void pkpy_node_event_animated_sprite_frame_changed(SESubjectNotifyPayload* payload) {
    const AnimatedSpriteFrameChangedPayload* eventPayload = (AnimatedSpriteFrameChangedPayload*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event_int(cre_pkpy_vm, (int)eventPayload->entity, CRE_PKPY_NODE_EVENT_NAME_FRAME_CHANGED, eventPayload->newFrame);
}

void pkpy_node_event_animated_sprite_animation_finished(SESubjectNotifyPayload* payload) {
    const AnimatedSpriteAnimationFinishedPayload* eventPayload = (AnimatedSpriteAnimationFinishedPayload*)payload->data;
    cre_pkpy_node_event_manager_broadcast_event_string(cre_pkpy_vm, (int)eventPayload->entity, CRE_PKPY_NODE_EVENT_NAME_ANIMATION_FINISHED, eventPayload->animation->name);
}
