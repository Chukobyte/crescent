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
