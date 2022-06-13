#include "py_script_context.h"

#include <Python.h>

#include "py_cache.h"
#include "../script_context.h"
#include "../../data_structures/rbe_hash_map.h"
#include "../../data_structures/rbe_static_array.h"
#include "../../utils/rbe_assert.h"

// --- Script Context Interface --- //
void py_on_create_instance(Entity entity, const char* classPath, const char* className);
void py_on_delete_instance(Entity entity);
void py_on_start(Entity entity);
void py_on_update_all_instances(float deltaTime);
void py_on_physics_update_all_instances(float deltaTime);
void py_on_end(Entity entity);

RBE_STATIC_ARRAY_CREATE(PyObject*, MAX_ENTITIES, entities_to_update);
RBE_STATIC_ARRAY_CREATE(PyObject*, MAX_ENTITIES, entities_to_physics_update);

RBEHashMap* pythonInstanceHashMap = NULL;

RBEScriptContext* rbe_py_create_script_context() {
    RBEScriptContext* scriptContext = rbe_script_context_create();
    scriptContext->on_create_instance = py_on_create_instance;
    scriptContext->on_delete_instance = py_on_delete_instance;
    scriptContext->on_start = py_on_start;
    scriptContext->on_update_all_instances = py_on_update_all_instances;
    scriptContext->on_physics_update_all_instances = py_on_physics_update_all_instances;
    scriptContext->on_end = py_on_end;

    pythonInstanceHashMap = rbe_hash_map_create(sizeof(Entity), sizeof(PyObject**), MAX_ENTITIES);
    return scriptContext;
}

void py_on_create_instance(Entity entity, const char* classPath, const char* className) {
    PyObject* pScriptInstance = rbe_py_cache_create_instance(classPath, className, entity);
    if (PyObject_HasAttrString(pScriptInstance, "_update")) {
        RBE_STATIC_ARRAY_ADD(entities_to_update, pScriptInstance);
    }
    if (PyObject_HasAttrString(pScriptInstance, "_physics_update")) {
        RBE_STATIC_ARRAY_ADD(entities_to_physics_update, pScriptInstance);
    }
    rbe_hash_map_add(pythonInstanceHashMap, &entity, &pScriptInstance);
}

void py_on_delete_instance(Entity entity) {
    RBE_ASSERT_FMT(rbe_hash_map_has(pythonInstanceHashMap, &entity), "Doesn't have entity '%d'", entity);
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    // Remove from update arrays
    if (PyObject_HasAttrString(pScriptInstance, "_update")) {
        RBE_STATIC_ARRAY_REMOVE(entities_to_update, pScriptInstance, NULL);
    }
    if (PyObject_HasAttrString(pScriptInstance, "_physics_update")) {
        RBE_STATIC_ARRAY_REMOVE(entities_to_physics_update, pScriptInstance, NULL);
    }

    Py_DecRef(pScriptInstance);
    rbe_hash_map_erase(pythonInstanceHashMap, &entity);
}

void py_on_start(Entity entity) {
    RBE_ASSERT_FMT(rbe_hash_map_has(pythonInstanceHashMap, &entity), "Tried to call py on_start to non existent python instance entity '%d'", entity);
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    RBE_ASSERT(pScriptInstance != NULL);
    if (PyObject_HasAttrString(pScriptInstance, "_start")) {
        PyObject_CallMethod(pScriptInstance, "_start", NULL);
    }
}

void py_on_update_all_instances(float deltaTime) {
    for (size_t i = 0; i < entities_to_update_count; i++) {
        RBE_ASSERT_FMT(entities_to_update[i] != NULL, "Python instance is null!");
        PyObject_CallMethod(entities_to_update[i], "_update", "(f)", deltaTime);
    }
    // TODO: More robust error checking
    PyErr_Print();
}

void py_on_physics_update_all_instances(float deltaTime) {
    for (size_t i = 0; i < entities_to_physics_update_count; i++) {
        RBE_ASSERT_FMT(entities_to_physics_update[i] != NULL, "Python instance is null!");
        PyObject_CallMethod(entities_to_physics_update[i], "_physics_update", "(f)", deltaTime);
    }
}

void py_on_end(Entity entity) {
    RBE_ASSERT(rbe_hash_map_has(pythonInstanceHashMap, &entity));
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    RBE_ASSERT(pScriptInstance != NULL);
    if (PyObject_HasAttrString(pScriptInstance, "_end")) {
        PyObject_CallMethod(pScriptInstance, "_end", NULL);
    }
}
