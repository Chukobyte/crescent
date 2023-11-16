#include "script_ec_system.h"

#include <Python.h>

#include <seika/data_structures/se_hash_map.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "ec_system.h"
#include "../../scene/scene_manager.h"
#include "../../scripting/python/py_script_context.h"
#include "../../scripting/native/native_script_context.h"
#include "../../scripting/native/internal_classes/fps_display_class.h"

void script_system_on_entity_registered(CreEntity entity);
void script_system_on_entity_unregistered(CreEntity entity);
void script_system_entity_start(CreEntity entity);
void script_system_entity_end(CreEntity entity);
void script_system_pre_update_all();
void script_system_post_update_all();
void script_system_instance_update(float deltaTime);
void script_system_instance_fixed_update(float deltaTime);
void script_system_network_callback(const char* message);
void script_system_on_ec_system_destroy();

CreEntitySystem* scriptSystem = NULL;

static CREScriptContext* scriptContexts[ScriptContextType_TOTAL_TYPES];
static size_t scriptContextsCount = 0;

CreEntitySystem* cre_script_ec_system_create() {
    SE_ASSERT(scriptSystem == NULL);
    scriptSystem = cre_ec_system_create();
    scriptSystem->name = se_strdup("Script");
    scriptSystem->on_entity_registered_func = script_system_on_entity_registered;
    scriptSystem->on_entity_unregistered_func = script_system_on_entity_unregistered;
    scriptSystem->on_entity_start_func = script_system_entity_start;
    scriptSystem->on_entity_end_func = script_system_entity_end;
    scriptSystem->pre_update_all_func = script_system_pre_update_all;
    scriptSystem->post_update_all_func = script_system_post_update_all;
    scriptSystem->update_func = script_system_instance_update;
    scriptSystem->fixed_update_func = script_system_instance_fixed_update;
    scriptSystem->network_callback_func = script_system_network_callback;
    scriptSystem->on_ec_system_destroy = script_system_on_ec_system_destroy;
    scriptSystem->component_signature = CreComponentType_SCRIPT;
    // Python Context
    scriptContexts[ScriptContextType_PYTHON] = cre_py_create_script_context();
    scriptContextsCount++;
    SE_ASSERT(scriptContexts[ScriptContextType_PYTHON] != NULL);
    // Native Context
    scriptContexts[ScriptContextType_NATIVE] = cre_native_create_script_context();
    scriptContextsCount++;
    // Register internal classed
    cre_native_class_register_new_class(fps_display_native_class_create_new());
    return scriptSystem;
}

void script_system_on_entity_registered(CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SCRIPT);
    SE_ASSERT(scriptContexts[scriptComponent->contextType] != NULL);
    SE_ASSERT(scriptContexts[scriptComponent->contextType]->on_create_instance != NULL);
    scriptContexts[scriptComponent->contextType]->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
}

void script_system_on_entity_unregistered(CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SCRIPT);
    scriptContexts[scriptComponent->contextType]->on_delete_instance(entity);
}

void script_system_entity_start(CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity,
            CreComponentDataIndex_SCRIPT);
    SE_ASSERT(scriptComponent != NULL);
    SE_ASSERT_FMT(scriptComponent->contextType == ScriptContextType_PYTHON || scriptComponent->contextType == ScriptContextType_NATIVE,
                  "Invalid context type '%d' for entity '%d'", scriptComponent->contextType, entity);
    scriptContexts[scriptComponent->contextType]->on_start(entity);
}

void script_system_entity_end(CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SCRIPT);
    scriptContexts[scriptComponent->contextType]->on_end(entity);
}

void script_system_pre_update_all() {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_pre_update_all != NULL) {
            scriptContexts[i]->on_pre_update_all();
        }
    }
}

void script_system_post_update_all() {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_post_update_all != NULL) {
            scriptContexts[i]->on_post_update_all();
        }
    }
}

void script_system_instance_update(float deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->updateEntityCount; entityIndex++) {
            const CreEntity entity = scriptContexts[i]->updateEntities[entityIndex];
            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            scriptContexts[i]->on_update_instance(entity, deltaTime * entityTimeDilation);
        }
    }
}

void script_system_instance_fixed_update(float deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->fixedUpdateEntityCount; entityIndex++) {
            const CreEntity entity = scriptContexts[i]->fixedUpdateEntities[entityIndex];
            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            scriptContexts[i]->on_fixed_update_instance(entity, deltaTime * entityTimeDilation);
        }
    }
}

void script_system_network_callback(const char* message) {
    // Hard coding python for now  TODO: Keep an array of script contexts that contain this callback
    scriptContexts[ScriptContextType_PYTHON]->on_network_callback(message);
}

void script_system_on_ec_system_destroy() {
    SE_ASSERT(scriptSystem != NULL);
    scriptSystem = NULL;

    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_script_context_destroy != NULL) {
            scriptContexts[i]->on_script_context_destroy();
        }
    }
    scriptContextsCount = 0;
}
