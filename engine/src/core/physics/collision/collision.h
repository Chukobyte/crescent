#pragma once

#include <stddef.h>

#include "../seika/src/data_structures/se_spatial_hash_map.h"

#include "../../ecs/entity/entity.h"

#define RBE_MAX_ENTITY_COLLISION 4

// TODO: Temp collision implementation, replace with a better one...

typedef struct CollisionResult {
    Entity sourceEntity;
    size_t collidedEntityCount;
    Entity collidedEntities[RBE_MAX_ENTITY_COLLISION];
} CollisionResult;

CollisionResult cre_collision_process_entity_collisions(Entity entity);
CollisionResult cre_collision_process_mouse_collisions(Rect2* collisionRect);
void cre_collision_set_global_spatial_hash_map(SESpatialHashMap* hashMap);
SESpatialHashMap* cre_collision_get_global_spatial_hash_map();
