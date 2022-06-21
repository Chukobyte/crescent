#include "py_script_context.h"

#include <Python.h>

#include "py_cache.h"
#include "../script_context.h"
#include "../../data_structures/rbe_hash_map.h"
#include "../../data_structures/rbe_static_array.h"
#include "../../utils/rbe_assert.h"
#include "../../memory/rbe_mem.h"
#include "../../networking/rbe_network.h"

//--- RBE Script Callback ---//
// TODO: Figuring out callback to signal structure. Clean up later.
typedef struct RBEScriptCallback {
    Entity entity;
    PyObject* callback_func;
} RBEScriptCallback;

void py_on_entity_subscribe_to_network_callback(Entity entity, PyObject* callback_func, const char* id);

static RBEScriptCallback* current_network_script_callback = NULL;
static RBEScriptCallback* current_network_server_client_connected_script_callback = NULL;

//--- Script Context Interface ---//
void py_on_create_instance(Entity entity, const char* classPath, const char* className);
void py_on_delete_instance(Entity entity);
void py_on_start(Entity entity);
void py_on_update_all_instances(float deltaTime);
void py_on_physics_update_all_instances(float deltaTime);
void py_on_end(Entity entity);
void py_on_network_callback(const char* message);

RBE_STATIC_ARRAY_CREATE(PyObject*, MAX_ENTITIES, entities_to_update);
RBE_STATIC_ARRAY_CREATE(PyObject*, MAX_ENTITIES, entities_to_physics_update);

RBEHashMap* pythonInstanceHashMap = NULL;
RBEScriptContext* python_script_context = NULL;

RBEScriptContext* rbe_py_create_script_context() {
    RBE_ASSERT(python_script_context == NULL);
    RBEScriptContext* scriptContext = rbe_script_context_create();
    scriptContext->on_create_instance = py_on_create_instance;
    scriptContext->on_delete_instance = py_on_delete_instance;
    scriptContext->on_start = py_on_start;
    scriptContext->on_update_all_instances = py_on_update_all_instances;
    scriptContext->on_physics_update_all_instances = py_on_physics_update_all_instances;
    scriptContext->on_end = py_on_end;
    scriptContext->on_network_callback = py_on_network_callback;
    scriptContext->on_entity_subscribe_to_network_callback = py_on_entity_subscribe_to_network_callback;

    pythonInstanceHashMap = rbe_hash_map_create(sizeof(Entity), sizeof(PyObject**), MAX_ENTITIES);
    python_script_context = scriptContext;
    return scriptContext;
}

RBEScriptContext* rbe_py_get_script_context() {
    return python_script_context;
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

    // Erase network callback
    if (current_network_server_client_connected_script_callback != NULL && entity == current_network_server_client_connected_script_callback->entity) {
        RBE_MEM_FREE(current_network_server_client_connected_script_callback);
        current_network_server_client_connected_script_callback = NULL;
    }
}

void py_on_start(Entity entity) {
    RBE_ASSERT_FMT(rbe_hash_map_has(pythonInstanceHashMap, &entity), "Tried to call py on_start to non existent python instance entity '%d'", entity);
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    RBE_ASSERT(pScriptInstance != NULL);
    if (PyObject_HasAttrString(pScriptInstance, "_start")) {
        PyObject_CallMethod(pScriptInstance, "_start", NULL);
        PyErr_Print();
    }
}

void py_on_update_all_instances(float deltaTime) {
    PyGILState_STATE pyGilStateState = PyGILState_Ensure();
    for (size_t i = 0; i < entities_to_update_count; i++) {
        RBE_ASSERT_FMT(entities_to_update[i] != NULL, "Python instance is null!");
        PyObject_CallMethod(entities_to_update[i], "_update", "(f)", deltaTime);
    }
    PyGILState_Release(pyGilStateState);

    // TODO: More robust error checking
    PyErr_Print();
}

void py_on_physics_update_all_instances(float deltaTime) {
    PyGILState_STATE pyGilStateState = PyGILState_Ensure();
    for (size_t i = 0; i < entities_to_physics_update_count; i++) {
        RBE_ASSERT_FMT(entities_to_physics_update[i] != NULL, "Python instance is null!");
        PyObject_CallMethod(entities_to_physics_update[i], "_physics_update", "(f)", deltaTime);
    }
    PyGILState_Release(pyGilStateState);
}

void py_on_end(Entity entity) {
    RBE_ASSERT(rbe_hash_map_has(pythonInstanceHashMap, &entity));
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) rbe_hash_map_get(pythonInstanceHashMap, &entity);
    RBE_ASSERT(pScriptInstance != NULL);
    if (PyObject_HasAttrString(pScriptInstance, "_end")) {
        PyObject_CallMethod(pScriptInstance, "_end", NULL);
    }
}

void py_on_network_callback(const char* message) {
//    rbe_logger_debug("py_on_network_callback - message = '%s'", message);
    if (current_network_script_callback != NULL) {
        PyObject* listenerFuncArg = Py_BuildValue("(s)", message);
        PyObject_CallObject(current_network_script_callback->callback_func, listenerFuncArg);
    }
}

// Entity Network Callback
void py_on_entity_subscribe_to_network_callback(Entity entity, PyObject* callback_func, const char* id) {
    rbe_logger_debug("py_on_entity_subscribe_to_network_callback");
    if (strcmp(id, "poll") == 0) {
        if (current_network_script_callback == NULL) {
            current_network_script_callback = RBE_MEM_ALLOCATE(RBEScriptCallback);
            current_network_script_callback->entity = entity;
            current_network_script_callback->callback_func = callback_func;
            Py_IncRef(current_network_script_callback->callback_func); // Increase ref to hold on to function
            Py_IncRef(current_network_script_callback->callback_func); // Why twice?
        }
    } else if (strcmp(id, "client_connected") == 0) {
        if (current_network_server_client_connected_script_callback == NULL) {
            rbe_udp_server_register_client_connected_callback(rbe_py_on_network_udp_server_client_connected);
            current_network_server_client_connected_script_callback = RBE_MEM_ALLOCATE(RBEScriptCallback);
            current_network_server_client_connected_script_callback->entity = entity;
            current_network_server_client_connected_script_callback->callback_func = callback_func;
            Py_IncRef(current_network_server_client_connected_script_callback->callback_func); // Increase ref to hold on to function
            Py_IncRef(current_network_server_client_connected_script_callback->callback_func); // Why twice?
        }
    }
}

void rbe_py_on_network_udp_server_client_connected() {
    if (current_network_server_client_connected_script_callback != NULL) {
        PyObject_CallObject(current_network_server_client_connected_script_callback->callback_func, NULL);
    }
}
