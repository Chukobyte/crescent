#include "script_ec_system.h"

#include <seika/assert.h>
#include <seika/logger.h>
#include <seika/data_structures/hash_map.h>
#include <seika/ecs/ecs.h>

#include "core/ecs/ecs_globals.h"
#include "core/ecs/components/script_component.h"
#include "core/scene/scene_manager.h"
#include "core/scripting/script_context.h"
#include "core/scripting/python/pocketpy/pkpy_script_context.h"
#include "core/scripting/native/native_script_context.h"

static void on_ec_system_registered(SkaECSSystem* system);
static void on_ec_system_destroyed(SkaECSSystem* system);
static void on_entity_registered(SkaECSSystem* system, SkaEntity entity);
static void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity);
static void on_entity_start(SkaECSSystem* system, SkaEntity entity);
static void on_entity_end(SkaECSSystem* system, SkaEntity entity);
static void on_pre_update_all(SkaECSSystem* system);
static void on_post_update_all(SkaECSSystem* system);
static void script_system_instance_update(SkaECSSystem* system, f32 deltaTime);
static void script_system_instance_fixed_update(SkaECSSystem* system, f32 deltaTime);
static void network_callback(SkaECSSystem* system, const char* message);

static CREScriptContext* scriptContexts[CreScriptContextType_TOTAL_TYPES];
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
    CREScriptContextTemplate templates[8];
    size_t templateCount = 0;

    templates[templateCount++] = cre_pkpy_get_script_context_template();
    templates[templateCount++] = cre_native_get_script_context_template();

    for (size_t i = 0; i < templateCount; i++) {
        const CREScriptContextTemplate* template = &templates[i];
        if (scriptContexts[template->contextType] != NULL) {
            ska_logger_warn("Attempted to override script context type '%d', ignoring!", template->contextType);
            continue;
        }
        scriptContexts[template->contextType] = cre_script_context_create_from_template(template);
        scriptContexts[template->contextType]->on_script_context_init(scriptContexts[template->contextType]);
        scriptContextsCount++;
    }
}

void on_ec_system_destroyed(SkaECSSystem* system) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        if (scriptContexts[i]->on_script_context_finalize != NULL) {
            scriptContexts[i]->on_script_context_finalize(scriptContexts[i]);
        }
    }
    scriptContextsCount = 0;
}

void on_entity_registered(SkaECSSystem* system, SkaEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    SKA_ASSERT(scriptComponent->contextType != CreScriptContextType_NONE);
    const CREScriptContext* scriptContext = scriptContexts[scriptComponent->contextType];
    SKA_ASSERT(scriptContext != NULL);
    SKA_ASSERT(scriptContext->on_create_instance != NULL);
    scriptContext->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
}

void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    scriptContexts[scriptComponent->contextType]->on_delete_instance(entity);
}

void on_entity_start(SkaECSSystem* system, SkaEntity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
    SKA_ASSERT_FMT(scriptComponent->contextType != CreScriptContextType_NONE, "Invalid context type '%d' for entity '%d'", scriptComponent->contextType, entity);
    scriptContexts[scriptComponent->contextType]->on_start(entity);
}

void on_entity_end(SkaECSSystem* system, SkaEntity entity) {
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

void script_system_instance_update(SkaECSSystem* system, f32 deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->updateEntityCount; entityIndex++) {
            const SkaEntity entity = scriptContexts[i]->updateEntities[entityIndex];
            const f32 entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            scriptContexts[i]->on_update_instance(entity, deltaTime * entityTimeDilation);
        }
    }
}

void script_system_instance_fixed_update(SkaECSSystem* system, f32 deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        for (size_t entityIndex = 0; entityIndex < scriptContexts[i]->fixedUpdateEntityCount; entityIndex++) {
            const SkaEntity entity = scriptContexts[i]->fixedUpdateEntities[entityIndex];
            const f32 entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            scriptContexts[i]->on_fixed_update_instance(entity, deltaTime * entityTimeDilation);
        }
    }
}

void network_callback(SkaECSSystem* system, const char* message) {
    // Hard coding python for now  TODO: Keep an array of script contexts that contain this callback
    scriptContexts[CreScriptContextType_PYTHON]->on_network_callback(message);
}
