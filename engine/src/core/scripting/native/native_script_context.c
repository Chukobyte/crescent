#include "native_script_context.h"

#include "../script_context.h"

// --- Script Context Interface --- //
void native_on_create_instance(Entity entity, const char* classPath, const char* className);
void native_on_delete_instance(Entity entity);
void native_on_start(Entity entity);
void native_on_update_all_instances(float deltaTime);
void native_on_physics_update_all_instances(float deltaTime);
void native_on_end(Entity entity);

RBEScriptContext* rbe_native_create_script_context() {
    RBEScriptContext* scriptContext = rbe_script_context_create();
    scriptContext->on_create_instance = native_on_create_instance;
    scriptContext->on_delete_instance = native_on_delete_instance;
    scriptContext->on_start = native_on_start;
    scriptContext->on_update_all_instances = native_on_update_all_instances;
    scriptContext->on_physics_update_all_instances = native_on_physics_update_all_instances;
    scriptContext->on_end = native_on_end;
    return scriptContext;
}

void native_on_create_instance(Entity entity, const char* classPath, const char* className) {}

void native_on_delete_instance(Entity entity) {}

void native_on_start(Entity entity) {}

void native_on_update_all_instances(float deltaTime) {}

void native_on_physics_update_all_instances(float deltaTime) {}

void native_on_end(Entity entity) {}
