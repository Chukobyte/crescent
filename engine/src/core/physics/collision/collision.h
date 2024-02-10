#pragma once

#include <stddef.h>

#include <seika/ecs/entity.h>
#include <seika/data_structures/se_spatial_hash_map.h>

#include "../../ecs/component/transform2d_component.h"
#include "../../ecs/component/collider2d_component.h"

#define CRE_MAX_ENTITY_COLLISION 8

// TODO: Temp collision implementation, replace with a better one...

typedef struct CollisionResult {
    SkaEntity sourceEntity;
    size_t collidedEntityCount;
    SkaEntity collidedEntities[CRE_MAX_ENTITY_COLLISION];
} CollisionResult;

CollisionResult cre_collision_process_entity_collisions(SkaEntity entity);
CollisionResult cre_collision_process_mouse_collisions(const SKARect2* collisionRect);
void cre_collision_set_global_spatial_hash_map(SESpatialHashMap* hashMap);
SESpatialHashMap* cre_collision_get_global_spatial_hash_map();
SKARect2 cre_get_collision_rectangle(SkaEntity entity, Transform2DComponent* transform2DComponent, Collider2DComponent* collider2DComponent);
