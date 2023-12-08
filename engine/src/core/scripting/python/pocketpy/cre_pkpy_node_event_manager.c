#include "cre_pkpy_node_event_manager.h"

#include <seika/utils/se_assert.h>

#include "cre_pkpy_util.h"

void cre_pkpy_node_event_manager_initialize(pkpy_vm* vm) {
    pkpy_exec(vm, "import crescent");
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
}

void cre_pkpy_node_event_manager_finalize() {}

bool cre_pkpy_node_event_manager_push_node_event_manager_broadcast_event_func(pkpy_vm* vm) {
    return pkpy_eval(vm, "crescent._node_event_manager.broadcast_event");
}

void cre_pkpy_node_event_manager_broadcast_event(pkpy_vm* vm, int ownerId, const char* eventName) {
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId); // Scene tree root
    pkpy_push_string(vm, pkpy_string(eventName));
    pkpy_vectorcall(vm, 2);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_bool(pkpy_vm* vm, int ownerId, const char* eventName, bool value) {
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId); // Scene tree root
    pkpy_push_string(vm, pkpy_string(eventName));
    pkpy_push_bool(vm, value);
    pkpy_vectorcall(vm, 3);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_int(pkpy_vm* vm, int ownerId, const char* eventName, int value) {
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId); // Scene tree root
    pkpy_push_string(vm, pkpy_string(eventName));
    pkpy_push_int(vm, value);
    pkpy_vectorcall(vm, 3);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_float(pkpy_vm* vm, int ownerId, const char* eventName, float value) {
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId); // Scene tree root
    pkpy_push_string(vm, pkpy_string(eventName));
    pkpy_push_float(vm, (double)value);
    pkpy_vectorcall(vm, 3);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}

void cre_pkpy_node_event_manager_broadcast_event_string(pkpy_vm* vm, int ownerId, const char* eventName, const char* value) {
    pkpy_push_null(vm);
    pkpy_push_int(vm, ownerId); // Scene tree root
    pkpy_push_string(vm, pkpy_string(eventName));
    pkpy_push_string(vm, pkpy_string(value));
    pkpy_vectorcall(vm, 3);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
}
