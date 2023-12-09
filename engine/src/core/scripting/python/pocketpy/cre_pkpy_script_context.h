#pragma once

#include "../../../ecs/entity/entity.h"

struct CREScriptContext* cre_pkpy_script_context_create();
struct CREScriptContext* cre_pkpy_script_context_get();

struct pkpy_vm_handle* cre_pkpy_script_context_get_active_pkpy_vm();
void cre_pkpy_script_context_on_network_udp_server_client_connected();
void cre_pkpy_script_context_create_instance(CreEntity entity, const char* classPath, const char* className);
void cre_pkpy_script_context_create_instance_if_nonexistent(CreEntity entity, const char* classPath, const char* className);
