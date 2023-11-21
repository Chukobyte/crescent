#include "cre_pkpy_script_context.h"

#include <pocketpy/pocketpy_c.h>

#include "../seika/utils/se_assert.h"
#include "../seika/memory/se_mem.h"
#include "../seika/networking/se_network.h"

#include "../../script_context.h"
#include "cre_pkpy_entity_instance_cache.h"

//--- Script Context Interface ---//
void pkpy_sc_on_create_instance(CreEntity entity, const char* classPath, const char* className);
void pkpy_sc_on_delete_instance(CreEntity entity);
void pkpy_sc_on_start(CreEntity entity);
void pkpy_sc_on_update_instance(CreEntity entity, float deltaTime);
void pkpy_sc_on_fixed_update_instance(CreEntity entity, float deltaTime);
void pkpy_sc_on_end(CreEntity entity);
void pkpy_sc_on_network_callback(const char* message);
void pkpy_sc_on_script_context_destroy();
//void pkpy_sc_on_entity_subscribe_to_network_callback(CreEntity entity, PyObject* callback_func, const char* id);

CREScriptContext* pkpy_script_context = NULL;
pkpy_vm* vm = NULL;

CREScriptContext* cre_pkpy_script_context_create() {
    SE_ASSERT_FMT(pkpy_script_context == NULL, "Script context already created!");
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = pkpy_sc_on_create_instance;
    scriptContext->on_delete_instance = pkpy_sc_on_delete_instance;
    scriptContext->on_start = pkpy_sc_on_start;
    scriptContext->on_update_instance = pkpy_sc_on_update_instance;
    scriptContext->on_fixed_update_instance = pkpy_sc_on_fixed_update_instance;
    scriptContext->on_end = pkpy_sc_on_end;
    scriptContext->on_network_callback = pkpy_sc_on_network_callback;
    scriptContext->on_script_context_destroy = pkpy_sc_on_script_context_destroy;
//    scriptContext->on_entity_subscribe_to_network_callback = pkpy_sc_on_entity_subscribe_to_network_callback;

    pkpy_script_context = scriptContext;

    if (!vm) {
        vm = pkpy_new_vm(false);
    }

    return scriptContext;
}

CREScriptContext* cre_pkpy_script_context_get() {
    return pkpy_script_context;
}

void pkpy_sc_on_create_instance(CreEntity entity, const char* classPath, const char* className) {
    cre_pkpy_entity_instance_cache_create_new_entity(vm, classPath, className, entity);
}

void pkpy_sc_on_delete_instance(CreEntity entity) {}

void pkpy_sc_on_start(CreEntity entity) {}

void pkpy_sc_on_update_instance(CreEntity entity, float deltaTime) {}

void pkpy_sc_on_fixed_update_instance(CreEntity entity, float deltaTime) {}

void pkpy_sc_on_end(CreEntity entity) {}

void pkpy_sc_on_network_callback(const char* message) {}

void pkpy_sc_on_script_context_destroy() {
    SE_ASSERT(vm != NULL);
    pkpy_delete_vm(vm);
    vm = NULL;

    SE_MEM_FREE(pkpy_script_context);
    pkpy_script_context = NULL;
}

// Entity Network Callback
//void pkpy_sc_on_entity_subscribe_to_network_callback(CreEntity entity, PyObject* callback_func, const char* id) {}

pkpy_vm* cre_pkpy_script_context_get_active_pkpy_vm() {
    return vm;
}

void cre_pkpy_script_context_on_network_udp_server_client_connected() {}
void cre_pkpy_script_context_create_script_instance(CreEntity entity, const char* classPath, const char* className) {}
void cre_pkpy_script_context_get_script_instance(CreEntity entity) {}
