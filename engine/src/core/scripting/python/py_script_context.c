#include "py_script_context.h"

#include <Python.h>

#include "../seika/src/data_structures/se_hash_map.h"
#include "../seika/src/data_structures/se_static_array.h"
#include "../seika/src/utils/se_assert.h"
#include "../seika/src/memory/se_mem.h"
#include "../seika/src/networking/se_network.h"

#include "py_cache.h"
#include "../script_context.h"
#include "../../node_event.h"

//--- RBE Script Callback ---//
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
void py_on_pre_update_all();
void py_on_post_update_all();
void py_on_update_instance(Entity entity, float deltaTime);
void py_on_physics_update_instance(Entity entity, float deltaTime);
void py_on_end(Entity entity);
void py_on_network_callback(const char* message);

SEHashMap* pythonInstanceHashMap = NULL;
CREScriptContext* python_script_context = NULL;
PyGILState_STATE pyGlobalGilStateState;

CREScriptContext* cre_py_create_script_context() {
    SE_ASSERT_FMT(python_script_context == NULL, "Script context already created!");
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = py_on_create_instance;
    scriptContext->on_delete_instance = py_on_delete_instance;
    scriptContext->on_start = py_on_start;
    scriptContext->on_pre_update_all = py_on_pre_update_all;
    scriptContext->on_post_update_all = py_on_post_update_all;
    scriptContext->on_update_instance = py_on_update_instance;
    scriptContext->on_physics_update_instance = py_on_physics_update_instance;
    scriptContext->on_end = py_on_end;
    scriptContext->on_network_callback = py_on_network_callback;
    scriptContext->on_entity_subscribe_to_network_callback = py_on_entity_subscribe_to_network_callback;

    pythonInstanceHashMap = se_hash_map_create(sizeof(Entity), sizeof(PyObject **), 16);
    python_script_context = scriptContext;
    return scriptContext;
}

CREScriptContext* cre_py_get_script_context() {
    return python_script_context;
}

void py_on_create_instance(Entity entity, const char* classPath, const char* className) {
    if (se_hash_map_has(pythonInstanceHashMap, &entity)) {
        se_logger_warn("Won't create entity '%d' with class_path '%s' and class_name '%s' as they already exist!", entity, classPath, className);
        return;
    }
    PyObject* pScriptInstance = cre_py_cache_create_instance(classPath, className, entity);
    se_hash_map_add(pythonInstanceHashMap, &entity, &pScriptInstance);
}

void py_on_delete_instance(Entity entity) {
    SE_ASSERT_FMT(se_hash_map_has(pythonInstanceHashMap, &entity), "Doesn't have entity '%d'", entity);
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) se_hash_map_get(pythonInstanceHashMap, &entity);
    // Remove from update arrays
    if (PyObject_HasAttrString(pScriptInstance, "_update")) {
        se_array_utils_remove_item_uint32(
            python_script_context->updateEntities,
            &python_script_context->updateEntityCount,
            entity,
            NULL_ENTITY
        );
    }
    if (PyObject_HasAttrString(pScriptInstance, "_physics_update")) {
        se_array_utils_remove_item_uint32(
            python_script_context->physicsUpdateEntities,
            &python_script_context->physicsUpdateEntityCount,
            entity,
            NULL_ENTITY
        );
    }

    Py_DecRef(pScriptInstance);
    se_hash_map_erase(pythonInstanceHashMap, &entity);

    // Erase network callback
    if (current_network_server_client_connected_script_callback != NULL && entity == current_network_server_client_connected_script_callback->entity) {
        SE_MEM_FREE(current_network_server_client_connected_script_callback);
        current_network_server_client_connected_script_callback = NULL;
    }
}

void py_on_start(Entity entity) {
    SE_ASSERT_FMT(se_hash_map_has(pythonInstanceHashMap, &entity), "Tried to call py on_start to non existent python instance entity '%d'", entity);
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) se_hash_map_get(pythonInstanceHashMap, &entity);
    SE_ASSERT(pScriptInstance != NULL);
    if (PyObject_HasAttrString(pScriptInstance, "_start")) {
        PyObject_CallMethod(pScriptInstance, "_start", NULL);
        PyErr_Print();
    }
    node_event_notify_observers(entity, "scene_entered", &(NodeEventNotifyPayload) {
        .data = pScriptInstance
    });
    if (PyObject_HasAttrString(pScriptInstance, "_update")) {
        python_script_context->updateEntities[python_script_context->updateEntityCount++] = entity;
    }
    if (PyObject_HasAttrString(pScriptInstance, "_physics_update")) {
        python_script_context->physicsUpdateEntities[python_script_context->physicsUpdateEntityCount++] = entity;
    }
}

void py_on_pre_update_all() {
    pyGlobalGilStateState = PyGILState_Ensure();
}

void py_on_post_update_all() {
    PyGILState_Release(pyGlobalGilStateState);
    PyErr_Print();
}

// TODO: Can get from an array instead of getting from a hashmap if performance becomes a problem.
void py_on_update_instance(Entity entity, float deltaTime) {
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) se_hash_map_get(pythonInstanceHashMap, &entity);
    PyObject_CallMethod(pScriptInstance, "_update", "(f)", deltaTime);
}

void py_on_physics_update_instance(Entity entity, float deltaTime) {
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) se_hash_map_get(pythonInstanceHashMap, &entity);
    PyObject_CallMethod(pScriptInstance, "_physics_update", "(f)", deltaTime);
}

void py_on_end(Entity entity) {
    SE_ASSERT(se_hash_map_has(pythonInstanceHashMap, &entity));
    PyObject* pScriptInstance = (PyObject*) *(PyObject**) se_hash_map_get(pythonInstanceHashMap, &entity);
    SE_ASSERT(pScriptInstance != NULL);
    node_event_notify_observers(entity, "scene_exited", &(NodeEventNotifyPayload) {
        .data = pScriptInstance
    });
    if (PyObject_HasAttrString(pScriptInstance, "_end")) {
        PyObject_CallMethod(pScriptInstance, "_end", NULL);
    }
}

void py_on_network_callback(const char* message) {
    if (current_network_script_callback != NULL) {
        // Ensuring gil state since this can be called outside of main processing loop
        PyGILState_STATE pyGilStateState = PyGILState_Ensure();
        PyObject* listenerFuncArg = Py_BuildValue("(s)", message);
        PyObject_CallObject(current_network_script_callback->callback_func, listenerFuncArg);
        PyGILState_Release(pyGilStateState);
    }
}

// Entity Network Callback
void py_on_entity_subscribe_to_network_callback(Entity entity, PyObject* callback_func, const char* id) {
    if (strcmp(id, "poll") == 0) {
        if (current_network_script_callback == NULL) {
            current_network_script_callback = SE_MEM_ALLOCATE(RBEScriptCallback);
            current_network_script_callback->entity = entity;
            current_network_script_callback->callback_func = callback_func;
            Py_IncRef(current_network_script_callback->callback_func); // Increase ref to hold on to function
            Py_IncRef(current_network_script_callback->callback_func); // Why twice?
        }
    } else if (strcmp(id, "client_connected") == 0) {
        if (current_network_server_client_connected_script_callback == NULL) {
            se_udp_server_register_client_connected_callback(cre_py_on_network_udp_server_client_connected);
            current_network_server_client_connected_script_callback = SE_MEM_ALLOCATE(RBEScriptCallback);
            current_network_server_client_connected_script_callback->entity = entity;
            current_network_server_client_connected_script_callback->callback_func = callback_func;
            Py_IncRef(current_network_server_client_connected_script_callback->callback_func); // Increase ref to hold on to function
            Py_IncRef(current_network_server_client_connected_script_callback->callback_func); // Why twice?
        }
    }
}

void cre_py_on_network_udp_server_client_connected() {
    if (current_network_server_client_connected_script_callback != NULL) {
        PyGILState_STATE pyGilStateState = PyGILState_Ensure();
        PyObject_CallObject(current_network_server_client_connected_script_callback->callback_func, NULL);
        PyGILState_Release(pyGilStateState);
    }
}

PyObject* cre_py_create_script_instance(Entity entity, const char* classPath, const char* className) {
    SE_ASSERT(python_script_context != NULL);
    python_script_context->on_create_instance(entity, classPath, className);
    return cre_py_get_script_instance(entity);
}


PyObject* cre_py_get_script_instance(Entity entity) {
    if (se_hash_map_has(pythonInstanceHashMap, &entity)) {
        return (PyObject*) *(PyObject**) se_hash_map_get(pythonInstanceHashMap, &entity);
    }
    return NULL;
}
