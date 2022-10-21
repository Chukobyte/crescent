#pragma once

#include "../../ecs/entity/entity.h"

struct CREScriptContext* cre_py_create_script_context();
struct CREScriptContext* cre_py_get_script_context();

void cre_py_on_network_udp_server_client_connected();
struct _object* cre_py_get_script_instance(Entity entity);
