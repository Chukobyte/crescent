#pragma once

#include "../../ecs/entity/entity.h"

struct RBEScriptContext* rbe_py_create_script_context();
struct RBEScriptContext* rbe_py_get_script_context();

void rbe_py_on_network_udp_server_client_connected();
struct _object* rbe_py_get_script_instance(Entity entity);  // TODO: Probably don't want to do this from py_module?
