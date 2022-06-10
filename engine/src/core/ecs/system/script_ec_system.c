#include "script_ec_system.h"

#include <Python.h>

#include "ec_system.h"
#include "../component/script_component.h"
#include "../../utils/rbe_assert.h"
#include "../../scripting/python/py_cache.h"
#include "../../data_structures/rbe_hash_map.h"

void script_system_on_entity_registered(Entity entity);
void script_system_on_entity_unregistered(Entity entity);

EntitySystem* scriptSystem = NULL;
static RBEHashMap* scriptInstanceHashMap = NULL;

EntitySystem* script_ec_system_create() {
    RBE_ASSERT(scriptSystem == NULL);
    scriptSystem = rbe_ec_system_create();
    scriptSystem->on_entity_registered_func = script_system_on_entity_registered;
    scriptSystem->on_entity_unregistered_func = script_system_on_entity_unregistered;
    RBE_ASSERT(scriptInstanceHashMap == NULL);
    scriptInstanceHashMap = rbe_hash_map_create(sizeof(Entity), sizeof(PyObject**), MAX_ENTITIES);
    return scriptSystem;
}

void script_system_on_entity_registered(Entity entity) {
    RBE_ASSERT_FMT(!rbe_hash_map_has(scriptInstanceHashMap, &entity), "Entity '%d' already exists in script ec system!", entity);
    ScriptComponent* scriptComponent = (ScriptComponent*) component_manager_get_component(entity, ComponentDataIndex_SCRIPT);
    PyObject* pScriptInstance = rbe_py_cache_create_instance(scriptComponent->classPath, scriptComponent->className, entity);
    rbe_hash_map_add(scriptInstanceHashMap, &entity, &pScriptInstance);
}

void script_system_on_entity_unregistered(Entity entity) {

}
