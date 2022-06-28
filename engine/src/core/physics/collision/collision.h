#pragma once

#include <stddef.h>
#include "../../ecs/entity/entity.h"

#define RBE_MAX_ENTITY_COLLISION 4

// TODO: Temp collision implementation, replace with a better one...

typedef struct CollisionResult {
    Entity sourceEntity;
    size_t collidedEntityCount;
    Entity collidedEntities[RBE_MAX_ENTITY_COLLISION];
} CollisionResult;

CollisionResult rbe_collision_process_entity_collisions(Entity entity);
