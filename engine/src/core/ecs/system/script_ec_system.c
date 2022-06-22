#include "script_ec_system.h"

#include <Python.h>

#include "ec_system.h"
#include "../../data_structures/rbe_hash_map.h"
#include "../../scripting/python/py_script_context.h"
#include "../../scripting/native/native_script_context.h"
#include "../../scripting/native/internal_classes/fps_display_class.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"

void script_system_on_entity_registered(Entity entity);
void script_system_on_entity_unregistered(Entity entity);
void script_system_entity_start(Entity entity);
void script_system_entity_end(Entity entity);
void script_system_instance_update(float deltaTime);
void script_system_instance_physics_update(float deltaTime);
void script_system_network_callback(const char* message);

EntitySystem* scriptSystem = NULL;

static RBEScriptContext* scriptContexts[ScriptContextType_TOTAL_TYPES];
static size_t scriptContextsCount = 0;

EntitySystem* script_ec_system_create() {
    RBE_ASSERT(scriptSystem == NULL);
    scriptSystem = rbe_ec_system_create();
    scriptSystem->name = rbe_strdup("Script");
    scriptSystem->on_entity_registered_func = script_system_on_entity_registered;
    scriptSystem->on_entity_unregistered_func = script_system_on_entity_unregistered;
    scriptSystem->on_entity_start_func = script_system_entity_start;
    scriptSystem->on_entity_end_func = script_system_entity_end;
    scriptSystem->process_func = script_system_instance_update;
    scriptSystem->physics_process_func = script_system_instance_physics_update;
    scriptSystem->network_callback_func = script_system_network_callback;
    scriptSystem->component_signature = ComponentType_SCRIPT;
    // Python Context
    scriptContexts[ScriptContextType_PYTHON] = rbe_py_create_script_context();
    scriptContextsCount++;
    RBE_ASSERT(scriptContexts[ScriptContextType_PYTHON] != NULL);
    // Native Context
    scriptContexts[ScriptContextType_NATIVE] = rbe_native_create_script_context();
    scriptContextsCount++;
    // Register internal classed
    rbe_native_class_register_new_class(fps_display_native_class_create_new());
    return scriptSystem;
}

void script_system_on_entity_registered(Entity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    RBE_ASSERT(scriptContexts[scriptComponent->contextType] != NULL);
    RBE_ASSERT(scriptContexts[scriptComponent->contextType]->on_create_instance != NULL);
    scriptContexts[scriptComponent->contextType]->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
}

void script_system_on_entity_unregistered(Entity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    scriptContexts[scriptComponent->contextType]->on_delete_instance(entity);
}

void script_system_entity_start(Entity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    RBE_ASSERT(scriptComponent != NULL);
    RBE_ASSERT_FMT(scriptComponent->contextType == ScriptContextType_PYTHON || scriptComponent->contextType == ScriptContextType_NATIVE,
                   "Invalid context type '%d' for entity '%d'", scriptComponent->contextType, entity);
    scriptContexts[scriptComponent->contextType]->on_start(entity);
}

void script_system_entity_end(Entity entity) {
    const ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    scriptContexts[scriptComponent->contextType]->on_end(entity);
}

void script_system_instance_update(float deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        scriptContexts[i]->on_update_all_instances(deltaTime);
    }
}

void script_system_instance_physics_update(float deltaTime) {
    for (size_t i = 0; i < scriptContextsCount; i++) {
        scriptContexts[i]->on_physics_update_all_instances(deltaTime);
    }
}

void script_system_network_callback(const char* message) {
    // Hard coding python for now
    scriptContexts[ScriptContextType_PYTHON]->on_network_callback(message);
}
