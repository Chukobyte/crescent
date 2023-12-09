#include "cre_pkpy_script_context.h"

#include <pocketpy/pocketpy_c.h>

#include "../seika/utils/se_assert.h"
#include "../seika/memory/se_mem.h"
#include "../seika/networking/se_network.h"

#include "../../script_context.h"
#include "cre_pkpy_entity_instance_cache.h"

//--- Script Context Interface ---//
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

pkpy_CName pyStartFunctionName;
pkpy_CName pyUpdateFunctionName;
pkpy_CName pyFixedUpdateFunctionName;
pkpy_CName pyEndFunctionName;

CREScriptContext* cre_pkpy_script_context_create() {
    SE_ASSERT_FMT(pkpy_script_context == NULL, "Script context already created!");
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = cre_pkpy_script_context_create_instance_if_nonexistent; // Using nonexistent version in case added to entity cache from elsewhere
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
        pyStartFunctionName = pkpy_name("_start");
        pyUpdateFunctionName = pkpy_name("_process");
        pyFixedUpdateFunctionName = pkpy_name("_fixed_process");
        pyEndFunctionName = pkpy_name("_end");
    }

    return scriptContext;
}

CREScriptContext* cre_pkpy_script_context_get() {
    return pkpy_script_context;
}

void cre_pkpy_script_context_create_instance(CreEntity entity, const char* classPath, const char* className) {
    SE_ASSERT(vm);
    cre_pkpy_entity_instance_cache_create_new_entity_if_nonexistent(vm, classPath, className, entity);
}

void cre_pkpy_script_context_create_instance_if_nonexistent(CreEntity entity, const char* classPath, const char* className) {
    SE_ASSERT(vm);
    if (!cre_pkpy_entity_instance_cache_has_entity(vm, entity)) {
        cre_pkpy_script_context_create_instance(entity, classPath, className);
    }
}

void pkpy_sc_on_delete_instance(CreEntity entity) {
    SE_ASSERT(vm);
    cre_pkpy_entity_instance_cache_remove_entity(vm, entity);
}

void pkpy_sc_on_start(CreEntity entity) {
    SE_ASSERT(vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
    if (pkpy_getattr(vm, pyStartFunctionName)) {
        pkpy_push_null(vm);
        pkpy_vectorcall(vm, 0);
    }
    pkpy_pop_top(vm);
}

void pkpy_sc_on_update_instance(CreEntity entity, float deltaTime) {
    SE_ASSERT(vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
    if (pkpy_getattr(vm, pyUpdateFunctionName)) {
        pkpy_push_null(vm);
        pkpy_vectorcall(vm, 0);
    }
    pkpy_pop_top(vm);
}

void pkpy_sc_on_fixed_update_instance(CreEntity entity, float deltaTime) {
    SE_ASSERT(vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
    if (pkpy_getattr(vm, pyFixedUpdateFunctionName)) {
        pkpy_push_null(vm);
        pkpy_vectorcall(vm, 0);
    }
    pkpy_pop_top(vm);
}

void pkpy_sc_on_end(CreEntity entity) {
    SE_ASSERT(vm);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
    if (pkpy_getattr(vm, pyEndFunctionName)) {
        pkpy_push_null(vm);
        pkpy_vectorcall(vm, 0);
    }
    pkpy_pop_top(vm);
}

void pkpy_sc_on_network_callback(const char* message) {
    SE_ASSERT(vm);
}

void pkpy_sc_on_script_context_destroy() {
    SE_ASSERT(vm);
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
