#pragma once

#include <pocketpy_c.h>

#include "../../../ecs/entity/entity.h"

// Assumes new vm instance
void cre_pypp_entity_instance_cache_initialize(pkpy_vm* vm);
void cre_pypp_entity_instance_cache_finalize(pkpy_vm* vm);
void cre_pypp_entity_instance_cache_add_entity(pkpy_vm* vm, CreEntity entity);
void cre_pypp_entity_instance_cache_remove_entity(pkpy_vm* vm, CreEntity entity);
void cre_pypp_entity_instance_cache_push_entity_instance(pkpy_vm* vm, CreEntity entity);
