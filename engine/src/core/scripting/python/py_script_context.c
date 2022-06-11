#include "py_script_context.h"

#include <Python.h>

#include "py_cache.h"
#include "../script_context.h"
#include "../../ecs/entity/entity.h"
#include "../../data_structures/rbe_hash_map.h"
#include "../../utils/rbe_assert.h"

// --- Script Context Interface --- //
void py_on_create_instance(Entity entity, const char* classPath, const char* className);
void py_on_delete_instance(Entity entity);
void py_on_start(Entity entity);
void py_on_update_all_instances(float deltaTime);
void py_on_end(Entity entity);

void py_remove_entity_from_update_array(Entity entityRemoved);

static PyObject* entitiesToUpdate[MAX_ENTITIES];
static size_t entitiesToUpdateCount = 0;
RBEHashMap* pythonInstanceHashMap = NULL;

RBEScriptContext* rbe_py_create_script_context() {
    RBEScriptContext* scriptContext = rbe_script_context_create();
    scriptContext->on_create_instance = py_on_create_instance;
    scriptContext->on_delete_instance = py_on_delete_instance;
    scriptContext->on_start = py_on_start;
    scriptContext->on_update_all_instances = py_on_update_all_instances;
    scriptContext->on_end = py_on_end;

    pythonInstanceHashMap = rbe_hash_map_create(sizeof(Entity), sizeof(PyObject**), MAX_ENTITIES);
    return scriptContext;
}

void py_on_create_instance(Entity entity, const char* classPath, const char* className) {
    PyObject* pScriptInstance = rbe_py_cache_create_instance(classPath, className, entity);
    if (PyObject_HasAttrString(pScriptInstance, "_update")) {
        entitiesToUpdate[entitiesToUpdateCount++] = pScriptInstance;
    }
    rbe_hash_map_add(pythonInstanceHashMap, &entity, &pScriptInstance);
}

void py_on_delete_instance(Entity entity) {
    RBE_ASSERT_FMT(rbe_hash_map_has(pythonInstanceHashMap, &entity), "Doesn't have entity '%d'", entity);
    entitiesToUpdateCount--;
    py_remove_entity_from_update_array(entity);
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    Py_DecRef(pScriptInstance);
    rbe_hash_map_erase(pythonInstanceHashMap, &entity);
}

void py_on_start(Entity entity) {
    RBE_ASSERT(rbe_hash_map_has(pythonInstanceHashMap, &entity));
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    RBE_ASSERT(pScriptInstance != NULL);
    if (PyObject_HasAttrString(pScriptInstance, "_start")) {
        PyObject_CallMethod(pScriptInstance, "_start", NULL);
    }
}

void py_on_update_all_instances(float deltaTime) {
    for (size_t i = 0; i < entitiesToUpdateCount; i++) {
        RBE_ASSERT_FMT(entitiesToUpdate[i] != NULL, "Python instance is null!");
        PyObject_CallMethod(entitiesToUpdate[i], "_update", "(f)", deltaTime);
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

void py_remove_entity_from_update_array(Entity entityRemoved) {
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entityRemoved);
    for (size_t i = 0; i < entitiesToUpdateCount; i++) {
        if (entitiesToUpdate[i] == pScriptInstance && i + 1 < entitiesToUpdateCount) {
            entitiesToUpdate[i] = entitiesToUpdate[i + 1];
            entitiesToUpdate[i + 1] = NULL;
        }
    }
}
