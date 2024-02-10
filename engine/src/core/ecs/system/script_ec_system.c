#include "script_ec_system.h"

#include <seika/data_structures/se_hash_map.h>
#include <seika/ecs/ecs.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "../ecs_globals.h"
#include "ec_system.h"
#include "../component/script_component.h"
#include "../../scene/scene_manager.h"
#include "../../scripting/script_context.h"
#include "../../scripting/python/pocketpy/cre_pkpy_script_context.h"
#include "../../scripting/native/native_script_context.h"
#include "../../scripting/native/internal_classes/fps_display_class.h"

static void on_ec_system_registered(SkaECSSystem* system);
static void on_ec_system_destroyed(SkaECSSystem* system);
static void on_entity_registered(SkaECSSystem* system, CreEntity entity);
static void on_entity_unregistered(SkaECSSystem* system, CreEntity entity);
static void on_entity_start(SkaECSSystem* system, CreEntity entity);
static void on_entity_end(SkaECSSystem* system, CreEntity entity);
static void on_pre_update_all(SkaECSSystem* system);
static void on_post_update_all(SkaECSSystem* system);
static void script_system_instance_update(SkaECSSystem* system, float deltaTime);
static void script_system_instance_fixed_update(SkaECSSystem* system, float deltaTime);
static void network_callback(SkaECSSystem* system, const char* message);

static CREScriptContext* scriptContexts[ScriptContextType_TOTAL_TYPES];
static size_t scriptContextsCount = 0;

void cre_script_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Script");
    systemTemplate.on_ec_system_register = on_ec_system_registered;
    systemTemplate.on_ec_system_destroy = on_ec_system_destroyed;
    systemTemplate.on_entity_registered_func = on_entity_registered;
    systemTemplate.on_entity_unregistered_func = on_entity_unregistered;
    systemTemplate.on_entity_start_func = on_entity_start;
    systemTemplate.on_entity_end_func = on_entity_end;
    systemTemplate.pre_update_all_func = on_pre_update_all;
    systemTemplate.post_update_all_func = on_post_update_all;
    systemTemplate.update_func = script_system_instance_update;
    systemTemplate.fixed_update_func = script_system_instance_fixed_update;
    systemTemplate.network_callback_func = network_callback;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, ScriptComponent);
}

void on_ec_system_registered(SkaECSSystem* system) {
    // Python Context
    scriptContexts[ScriptContextType_PYTHON] = cre_pkpy_script_context_create();
    scriptContextsCount++;
    SE_ASSERT(scriptContexts[ScriptContextType_PYTHON] != NULL);
    // Native Context
    scriptContexts[ScriptContextType_NATIVE] = cre_native_create_script_context();
    scriptContextsCount++;
    // Register internal classed
    cre_native_class_register_new_class(fps_display_native_class_create_new());
}

void on_ec_system_destroyed(SkaECSSystem* system) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_script_context_destroy != NULL) {
            scriptContexts[i]->on_script_context_destroy();
        }
    }
    scriptContextsCount = 0;
}

void on_entity_registered(SkaECSSystem* system, CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    SE_ASSERT(scriptComponent->contextType != ScriptContextType_NONE);
    const CREScriptContext* scriptContext = scriptContexts[scriptComponent->contextType];
    SE_ASSERT(scriptContext != NULL);
    SE_ASSERT(scriptContext->on_create_instance != NULL);
    scriptContext->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
}

void on_entity_unregistered(SkaECSSystem* system, CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    scriptContexts[scriptComponent->contextType]->on_delete_instance(entity);
}

void on_entity_start(SkaECSSystem* system, CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    SE_ASSERT_FMT(scriptComponent->contextType != ScriptContextType_NONE, "Invalid context type '%d' for entity '%d'", scriptComponent->contextType, entity);
    scriptContexts[scriptComponent->contextType]->on_start(entity);
}

void on_entity_end(SkaECSSystem* system, CreEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    scriptContexts[scriptComponent->contextType]->on_end(entity);
}

void on_pre_update_all(SkaECSSystem* system) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_pre_update_all != NULL) {
            scriptContexts[i]->on_pre_update_all();
        }
    }
}

void on_post_update_all(SkaECSSystem* system) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_post_update_all != NULL) {
            scriptContexts[i]->on_post_update_all();
        }
    }
}

void script_system_instance_update(SkaECSSystem* system, float deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->updateEntityCount; entityIndex++) {
            const CreEntity entity = scriptContexts[i]->updateEntities[entityIndex];
            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            scriptContexts[i]->on_update_instance(entity, deltaTime * entityTimeDilation);
        }
    }
}

void script_system_instance_fixed_update(SkaECSSystem* system, float deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->fixedUpdateEntityCount; entityIndex++) {
            const CreEntity entity = scriptContexts[i]->fixedUpdateEntities[entityIndex];
            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            scriptContexts[i]->on_fixed_update_instance(entity, deltaTime * entityTimeDilation);
        }
    }
}

void network_callback(SkaECSSystem* system, const char* message) {
    // Hard coding python for now  TODO: Keep an array of script contexts that contain this callback
    scriptContexts[ScriptContextType_PYTHON]->on_network_callback(message);
}

// OLD
//void script_system_on_entity_registered(CreEntity entity);
//void script_system_on_entity_unregistered(CreEntity entity);
//void script_system_entity_start(CreEntity entity);
//void script_system_entity_end(CreEntity entity);
//void script_system_pre_update_all();
//void script_system_post_update_all();
//void script_system_instance_update(float deltaTime);
//void script_system_instance_fixed_update(float deltaTime);
//void script_system_network_callback(const char* message);
//void script_system_on_ec_system_destroy();
//
//CreEntitySystem* scriptSystem = NULL;
//
//static CREScriptContext* scriptContexts[ScriptContextType_TOTAL_TYPES];
//static size_t scriptContextsCount = 0;
//
//CreEntitySystem* cre_script_ec_system_create() {
//    SE_ASSERT(scriptSystem == NULL);
//    scriptSystem = cre_ec_system_create();
//    scriptSystem->name = se_strdup("Script");
//    scriptSystem->on_entity_registered_func = script_system_on_entity_registered;
//    scriptSystem->on_entity_unregistered_func = script_system_on_entity_unregistered;
//    scriptSystem->on_entity_start_func = script_system_entity_start;
//    scriptSystem->on_entity_end_func = script_system_entity_end;
//    scriptSystem->pre_update_all_func = script_system_pre_update_all;
//    scriptSystem->post_update_all_func = script_system_post_update_all;
//    scriptSystem->update_func = script_system_instance_update;
//    scriptSystem->fixed_update_func = script_system_instance_fixed_update;
//    scriptSystem->network_callback_func = script_system_network_callback;
//    scriptSystem->on_ec_system_destroy = script_system_on_ec_system_destroy;
//    scriptSystem->component_signature = CreComponentType_SCRIPT;
//    // Python Context
//    scriptContexts[ScriptContextType_PYTHON] = cre_pkpy_script_context_create();
//    scriptContextsCount++;
//    SE_ASSERT(scriptContexts[ScriptContextType_PYTHON] != NULL);
//    // Native Context
//    scriptContexts[ScriptContextType_NATIVE] = cre_native_create_script_context();
//    scriptContextsCount++;
//    // Register internal classed
//    cre_native_class_register_new_class(fps_display_native_class_create_new());
//    return scriptSystem;
//}
//
//void script_system_on_entity_registered(CreEntity entity) {
//    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SCRIPT);
//    SE_ASSERT(scriptComponent->contextType != ScriptContextType_NONE);
//    const CREScriptContext* scriptContext = scriptContexts[scriptComponent->contextType];
//    SE_ASSERT(scriptContext != NULL);
//    SE_ASSERT(scriptContext->on_create_instance != NULL);
//    scriptContext->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
//}
//
//void script_system_on_entity_unregistered(CreEntity entity) {
//    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SCRIPT);
//    scriptContexts[scriptComponent->contextType]->on_delete_instance(entity);
//}
//
//void script_system_entity_start(CreEntity entity) {
//    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity,
//            CreComponentDataIndex_SCRIPT);
//    SE_ASSERT(scriptComponent != NULL);
//    SE_ASSERT_FMT(scriptComponent->contextType != ScriptContextType_NONE, "Invalid context type '%d' for entity '%d'", scriptComponent->contextType, entity);
//    scriptContexts[scriptComponent->contextType]->on_start(entity);
//}
//
//void script_system_entity_end(CreEntity entity) {
//    const ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SCRIPT);
//    scriptContexts[scriptComponent->contextType]->on_end(entity);
//}
//
//void script_system_pre_update_all() {
//    for (size_t i = 0; i < scriptContextsCount; i++) {
//        if (scriptContexts[i]->on_pre_update_all != NULL) {
//            scriptContexts[i]->on_pre_update_all();
//        }
//    }
//}
//
//void script_system_post_update_all() {
//    for (size_t i = 0; i < scriptContextsCount; i++) {
//        if (scriptContexts[i]->on_post_update_all != NULL) {
//            scriptContexts[i]->on_post_update_all();
//        }
//    }
//}
//
//void script_system_instance_update(float deltaTime) {
//    for (size_t i = 0; i < scriptContextsCount; i++) {
//        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->updateEntityCount; entityIndex++) {
//            const CreEntity entity = scriptContexts[i]->updateEntities[entityIndex];
//            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
//            scriptContexts[i]->on_update_instance(entity, deltaTime * entityTimeDilation);
//        }
//    }
//}
//
//void script_system_instance_fixed_update(float deltaTime) {
//    for (size_t i = 0; i < scriptContextsCount; i++) {
//        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->fixedUpdateEntityCount; entityIndex++) {
//            const CreEntity entity = scriptContexts[i]->fixedUpdateEntities[entityIndex];
//            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
//            scriptContexts[i]->on_fixed_update_instance(entity, deltaTime * entityTimeDilation);
//        }
//    }
//}
//
//void script_system_network_callback(const char* message) {
//    // Hard coding python for now  TODO: Keep an array of script contexts that contain this callback
//    scriptContexts[ScriptContextType_PYTHON]->on_network_callback(message);
//}
//
//void script_system_on_ec_system_destroy() {
//    SE_ASSERT(scriptSystem != NULL);
//    scriptSystem = NULL;
//
//    for (size_t i = 0; i < scriptContextsCount; i++) {
//        if (scriptContexts[i]->on_script_context_destroy != NULL) {
//            scriptContexts[i]->on_script_context_destroy();
//        }
//    }
//    scriptContextsCount = 0;
//}
