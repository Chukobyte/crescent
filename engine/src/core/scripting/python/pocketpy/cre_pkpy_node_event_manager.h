#pragma once

#include <stdbool.h>

struct pkpy_vm_handle;

void cre_pkpy_node_event_manager_initialize(struct pkpy_vm_handle* vm);
void cre_pkpy_node_event_manager_finalize();
bool cre_pkpy_node_event_manager_push_node_event_manager_broadcast_event_func(struct pkpy_vm_handle* vm);
