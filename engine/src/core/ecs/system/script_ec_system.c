#include "script_ec_system.h"

#include <Python.h>

#include "ec_system.h"
#include "../../data_structures/rbe_hash_map.h"
#include "../../scripting/python/py_script_context.h"
#include "../../utils/rbe_assert.h"

#define MAX_SCRIPT_CONTEXTS 4

void script_system_on_entity_registered(Entity entity);
void script_system_on_entity_unregistered(Entity entity);
void script_system_instance_update(float deltaTime);
void script_system_instance_physics_update(float deltaTime);

EntitySystem* scriptSystem = NULL;
RBEScriptContext* scriptContext = NULL;

//static RBEScriptContext* scriptContexts[MAX_SCRIPT_CONTEXTS];
//static size_t scriptContextsCount;

EntitySystem* script_ec_system_create() {
    RBE_ASSERT(scriptSystem == NULL);
    scriptSystem = rbe_ec_system_create();
    scriptSystem->on_entity_registered_func = script_system_on_entity_registered;
    scriptSystem->on_entity_unregistered_func = script_system_on_entity_unregistered;
    scriptSystem->process_func = script_system_instance_update;
    scriptSystem->physics_process_func = script_system_instance_physics_update;
    scriptContext = rbe_py_create_script_context();
    RBE_ASSERT(scriptContext != NULL);
    return scriptSystem;
}

void script_system_on_entity_registered(Entity entity) {
    ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    scriptContext->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
}

void script_system_on_entity_unregistered(Entity entity) {
    scriptContext->on_delete_instance(entity);
}

void script_system_instance_update(float deltaTime) {
    scriptContext->on_update_all_instances(deltaTime);
}

void script_system_instance_physics_update(float deltaTime) {
    scriptContext->on_physics_update_all_instances(deltaTime);
}
