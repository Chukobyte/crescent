#pragma once

#include <stdbool.h>

struct pkpy_vm_handle;

void cre_pkpy_node_event_manager_initialize(struct pkpy_vm_handle* vm);
void cre_pkpy_node_event_manager_finalize();
bool cre_pkpy_node_event_manager_push_node_event_manager_broadcast_event_func(struct pkpy_vm_handle* vm);

// Broadcast event functions

void cre_pkpy_node_event_manager_broadcast_event(struct pkpy_vm_handle* vm, int ownerId, const char* eventName);
void cre_pkpy_node_event_manager_broadcast_event_bool(struct pkpy_vm_handle* vm, int ownerId, const char* eventName, bool value);
void cre_pkpy_node_event_manager_broadcast_event_int(struct pkpy_vm_handle* vm, int ownerId, const char* eventName, int value);
void cre_pkpy_node_event_manager_broadcast_event_float(struct pkpy_vm_handle* vm, int ownerId, const char* eventName, float value);
void cre_pkpy_node_event_manager_broadcast_event_string(struct pkpy_vm_handle* vm, int ownerId, const char* eventName, const char* value);

// Helper broadcast event macros

#define CRE_PKPY_NODE_EVENT_MANAGER_BROADCAST(PKPY_VM, OWNER_ID, EVENT_NAME) \
cre_pkpy_node_event_manager_broadcast_event(PKPY_VM, OWNER_ID, EVENT_NAME)

#define CRE_PKPY_NODE_EVENT_MANAGER_BROADCAST_ONE_PARAM(PKPY_VM, OWNER_ID, EVENT_NAME, PARAM) _Generic((PARAM), \
    bool: cre_pkpy_node_event_manager_broadcast_event_bool(PKPY_VM, OWNER_ID, EVENT_NAME, PARAM), \
    int: cre_pkpy_node_event_manager_broadcast_event_int(PKPY_VM, OWNER_ID, EVENT_NAME, PARAM), \
    float: cre_pkpy_node_event_manager_broadcast_event_float(PKPY_VM, OWNER_ID, EVENT_NAME, PARAM), \
    char*: cre_pkpy_node_event_manager_broadcast_event_string(PKPY_VM, OWNER_ID, EVENT_NAME, PARAM) \
)
