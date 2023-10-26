#pragma once

#include "../../../ecs/entity/entity.h"

struct CREScriptContext* cre_py_pp_create_script_context();
struct CREScriptContext* cre_py_pp_get_script_context();

void cre_py_pp_on_network_udp_server_client_connected();
void cre_py_pp_create_script_instance(CreEntity entity, const char* classPath, const char* className);
void cre_py_pp_get_script_instance(CreEntity entity);
