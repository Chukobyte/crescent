#pragma once

#include <pocketpy_c.h>

#include "../../../ecs/entity/entity.h"

// Assumes new vm instance
void cre_pkpy_entity_instance_cache_initialize(pkpy_vm* vm);
void cre_pkpy_entity_instance_cache_finalize(pkpy_vm* vm);
CreEntity cre_pkpy_entity_instance_cache_create_new_entity(pkpy_vm* vm, const char* classPath, const char* className, CreEntity entity);
// Returns true if an entity is created
bool cre_pkpy_entity_instance_cache_create_new_entity_if_nonexistent(pkpy_vm* vm, const char* classPath, const char* className, CreEntity entity);
void cre_pkpy_entity_instance_cache_remove_entity(pkpy_vm* vm, CreEntity entity);
bool cre_pkpy_entity_instance_cache_has_entity(pkpy_vm* vm, CreEntity entity);
void cre_pkpy_entity_instance_cache_push_entity_instance(pkpy_vm* vm, CreEntity entity);
// Will create entity instance.  If no script component is attached, it's assumed its a base class and will create one of those and attach a script component
void cre_pkpy_entity_instance_cache_add_if_nonexistent_and_push_entity_instance(pkpy_vm* vm, CreEntity entity);
