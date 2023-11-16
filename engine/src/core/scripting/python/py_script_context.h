#pragma once

#include "../../ecs/entity/entity.h"

struct CREScriptContext* cre_py_create_script_context();
struct CREScriptContext* cre_py_get_script_context();

struct _object* cre_py_create_script_instance(CreEntity entity, const char* classPath, const char* className);
struct _object* cre_py_get_script_instance(CreEntity entity);
