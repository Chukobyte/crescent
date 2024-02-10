#pragma once

#include <seika/ecs/entity.h>

struct CREScriptContext* cre_pkpy_script_context_create();
struct CREScriptContext* cre_pkpy_script_context_get();

struct pkpy_vm_handle* cre_pkpy_script_context_get_active_pkpy_vm();
void cre_pkpy_script_context_on_network_udp_server_client_connected();
// All instances of script instance creation should be routed through one of these functions to setup things properly
void cre_pkpy_script_context_create_instance(SkaEntity entity, const char* classPath, const char* className);
void cre_pkpy_script_context_create_instance_if_nonexistent(SkaEntity entity, const char* classPath, const char* className);
void cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(SkaEntity entity);
