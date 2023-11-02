#include "cre_py_script_context.h"

#include <Python.h>

#include "../seika/src/utils/se_assert.h"
#include "../seika/src/memory/se_mem.h"
#include "../seika/src/networking/se_network.h"

#include "../../script_context.h"

//--- Script Context Interface ---//
void py_pp_on_create_instance(CreEntity entity, const char* classPath, const char* className);
void py_pp_on_delete_instance(CreEntity entity);
void py_pp_on_start(CreEntity entity);
void py_pp_on_update_instance(CreEntity entity, float deltaTime);
void py_pp_on_fixed_update_instance(CreEntity entity, float deltaTime);
void py_pp_on_end(CreEntity entity);
void py_pp_on_network_callback(const char* message);
void py_pp_on_entity_subscribe_to_network_callback(CreEntity entity, PyObject* callback_func, const char* id);

CREScriptContext* pp_script_context = NULL;

CREScriptContext* cre_pkpy_script_context_create() {
    SE_ASSERT_FMT(pp_script_context == NULL, "Script context already created!");
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = py_pp_on_create_instance;
    scriptContext->on_delete_instance = py_pp_on_delete_instance;
    scriptContext->on_start = py_pp_on_start;
    scriptContext->on_update_instance = py_pp_on_update_instance;
    scriptContext->on_fixed_update_instance = py_pp_on_fixed_update_instance;
    scriptContext->on_end = py_pp_on_end;
    scriptContext->on_network_callback = py_pp_on_network_callback;
    scriptContext->on_entity_subscribe_to_network_callback = py_pp_on_entity_subscribe_to_network_callback;

    pp_script_context = scriptContext;
    return scriptContext;
}

CREScriptContext* cre_pkpy_script_context_get() {
    return pp_script_context;
}

void py_pp_on_create_instance(CreEntity entity, const char* classPath, const char* className) {}
void py_pp_on_delete_instance(CreEntity entity) {}
void py_pp_on_start(CreEntity entity) {}
void py_pp_on_update_instance(CreEntity entity, float deltaTime) {}
void py_pp_on_fixed_update_instance(CreEntity entity, float deltaTime) {}
void py_pp_on_end(CreEntity entity) {}
void py_pp_on_network_callback(const char* message) {}

// Entity Network Callback
void py_pp_on_entity_subscribe_to_network_callback(CreEntity entity, PyObject* callback_func, const char* id) {}
void cre_pkpy_script_context_on_network_udp_server_client_connected() {}
void cre_pkpy_script_context_create_script_instance(CreEntity entity, const char* classPath, const char* className) {}
void cre_pkpy_script_context_get_script_instance(CreEntity entity) {}
