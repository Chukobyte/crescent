#pragma once

#include <stdbool.h>

#include <seika/defines.h>

struct pkpy_vm_handle;

void cre_pkpy_node_event_manager_initialize(struct pkpy_vm_handle* vm);
void cre_pkpy_node_event_manager_finalize();

void cre_pkpy_node_event_manager_create_event(struct pkpy_vm_handle* vm, int32 ownerId, const char* eventName);
void cre_pkpy_node_event_manager_remove_entity_and_connections(struct pkpy_vm_handle* vm, int32 ownerId);

// Broadcast event functions

bool cre_pkpy_node_event_manager_push_broadcast_event_func(struct pkpy_vm_handle *vm);
void cre_pkpy_node_event_manager_broadcast_event(struct pkpy_vm_handle* vm, int32 ownerId, const char* eventName);
void cre_pkpy_node_event_manager_broadcast_event_bool(struct pkpy_vm_handle* vm, int32 ownerId, const char* eventName, bool value);
void cre_pkpy_node_event_manager_broadcast_event_int(struct pkpy_vm_handle* vm, int32 ownerId, const char* eventName, int32 value);
void cre_pkpy_node_event_manager_broadcast_event_float(struct pkpy_vm_handle* vm, int32 ownerId, const char* eventName, f32 value);
void cre_pkpy_node_event_manager_broadcast_event_string(struct pkpy_vm_handle* vm, int32 ownerId, const char* eventName, const char* value);
