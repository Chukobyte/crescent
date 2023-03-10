#pragma once

#include <stddef.h>

#include "../seika/src/data_structures/se_spatial_hash_map.h"

#include "../../ecs/entity/entity.h"
#include "../../ecs/component/transform2d_component.h"
#include "../../ecs/component/collider2d_component.h"

#define CRE_MAX_ENTITY_COLLISION 8

// TODO: Temp collision implementation, replace with a better one...

typedef struct CollisionResult {
    CreEntity sourceEntity;
    size_t collidedEntityCount;
    CreEntity collidedEntities[CRE_MAX_ENTITY_COLLISION];
} CollisionResult;

CollisionResult cre_collision_process_entity_collisions(CreEntity entity);
CollisionResult cre_collision_process_mouse_collisions(SERect2* collisionRect);
void cre_collision_set_global_spatial_hash_map(SESpatialHashMap* hashMap);
SESpatialHashMap* cre_collision_get_global_spatial_hash_map();
SERect2 cre_get_collision_rectangle(CreEntity entity, Transform2DComponent* transform2DComponent, Collider2DComponent* collider2DComponent);
