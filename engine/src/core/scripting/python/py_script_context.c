#include "py_script_context.h"

#include <Python.h>

#include "py_cache.h"
#include "../script_context.h"
#include "../../ecs/entity/entity.h"

void py_on_create_instance(Entity entity, const char* classPath, const char* className);
void py_on_delete_instance(Entity entity);
void py_on_start(Entity entity);
void py_on_update(Entity entity, float deltaTime);
void py_on_end(Entity entity);

RBEScriptContext* rbe_py_create_script_context() {
    RBEScriptContext* scriptContext = rbe_script_context_create();
    scriptContext->on_create_instance = py_on_create_instance;
    scriptContext->on_delete_instance = py_on_delete_instance;
    scriptContext->on_start = py_on_start;
    scriptContext->on_update = py_on_update;
    scriptContext->on_end = py_on_end;
    return scriptContext;
}

void py_on_create_instance(Entity entity, const char* classPath, const char* className) {
    PyObject* pScriptInstance = rbe_py_cache_create_instance(classPath, className, entity);
}

void py_on_delete_instance(Entity entity) {}

void py_on_start(Entity entity) {}

void py_on_update(Entity entity, float deltaTime) {}

void py_on_end(Entity entity) {}
