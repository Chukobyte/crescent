#include "script_ec_system.h"

#include <Python.h>

#include "ec_system.h"
#include "../component/script_component.h"
#include "../../utils/rbe_assert.h"
#include "../../data_structures/rbe_hash_map.h"

void script_system_on_entity_registered(Entity entity);
void script_system_on_entity_unregistered(Entity entity);

EntitySystem* scriptSystem = NULL;
RBEScriptContext* scriptContext = NULL;

EntitySystem* script_ec_system_create(RBEScriptContext* newScriptContext) {
    RBE_ASSERT(scriptSystem == NULL);
    scriptSystem = rbe_ec_system_create();
    scriptSystem->on_entity_registered_func = script_system_on_entity_registered;
    scriptSystem->on_entity_unregistered_func = script_system_on_entity_unregistered;
    RBE_ASSERT(newScriptContext != NULL);
    scriptContext = newScriptContext;
    return scriptSystem;
}

void script_system_on_entity_registered(Entity entity) {
    ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    scriptContext->on_create_instance(entity, scriptComponent->classPath, scriptComponent->className);
    scriptContext->on_start(entity); // TODO: Move this once there is a place for it especially during stage creation
}

void script_system_on_entity_unregistered(Entity entity) {
    scriptContext->on_delete_instance(entity);
}
