#include "cre_pkpy_node_event_manager.h"

#include <seika/assert.h>

#include "cre_pkpy_util.h"


static inline void cre_pkpy_node_event_manager_push_broadcast_and_initial_args(pkpy_vm* vm, int ownerId, const char* eventName) {
    cre_pkpy_node_event_manager_push_broadcast_event_func(vm);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId);
    pkpy_push_string(vm, pkpy_string(eventName));
}

static inline bool cre_pkpy_node_event_manager_push_create_event_func(pkpy_vm* vm) {
    return pkpy_eval(vm, "crescent._node_event_manager.create_event");
}

static inline bool cre_pkpy_node_event_manager_push_remove_entity_and_connections_func(pkpy_vm* vm) {
    return pkpy_eval(vm, "crescent._node_event_manager.remove_entity_and_connections");
}

void cre_pkpy_node_event_manager_initialize(pkpy_vm* vm) {
    pkpy_exec(vm, "import crescent");
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
}

void cre_pkpy_node_event_manager_finalize() {}

void cre_pkpy_node_event_manager_create_event(pkpy_vm* vm, int ownerId, const char* eventName) {
    cre_pkpy_node_event_manager_push_create_event_func(vm);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId);
    pkpy_push_string(vm, pkpy_string(eventName));
    pkpy_vectorcall(vm, 2);
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_remove_entity_and_connections(pkpy_vm* vm, int ownerId) {
    cre_pkpy_node_event_manager_push_remove_entity_and_connections_func(vm);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId);
    pkpy_vectorcall(vm, 1);
    pkpy_pop_top(vm);
}

bool cre_pkpy_node_event_manager_push_broadcast_event_func(pkpy_vm* vm) {
    return pkpy_eval(vm, "crescent._node_event_manager.broadcast_event");
}

void cre_pkpy_node_event_manager_broadcast_event(pkpy_vm* vm, int ownerId, const char* eventName) {
    cre_pkpy_node_event_manager_push_broadcast_and_initial_args(vm, ownerId, eventName);
    pkpy_vectorcall(vm, 2);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_bool(pkpy_vm* vm, int ownerId, const char* eventName, bool value) {
    cre_pkpy_node_event_manager_push_broadcast_and_initial_args(vm, ownerId, eventName);
    pkpy_push_bool(vm, value);
    pkpy_vectorcall(vm, 3);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_int(pkpy_vm* vm, int ownerId, const char* eventName, int value) {
    cre_pkpy_node_event_manager_push_broadcast_and_initial_args(vm, ownerId, eventName);
    pkpy_push_int(vm, value);
    pkpy_vectorcall(vm, 3);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_float(pkpy_vm* vm, int ownerId, const char* eventName, float value) {
    cre_pkpy_node_event_manager_push_broadcast_and_initial_args(vm, ownerId, eventName);
    pkpy_push_float(vm, (double)value);
    pkpy_vectorcall(vm, 3);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_string(pkpy_vm* vm, int ownerId, const char* eventName, const char* value) {
    cre_pkpy_node_event_manager_push_broadcast_and_initial_args(vm, ownerId, eventName);
    pkpy_push_string(vm, pkpy_string(value));
    pkpy_vectorcall(vm, 3);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}
