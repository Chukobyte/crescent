#include "collision.h"

#include "../seika/src/utils/logger.h"
#include "../seika/src/utils/se_assert.h"

#include "../../ecs/system/ec_system.h"
#include "../../ecs/system/collision_ec_system.h"
#include "../../scene/scene_manager.h"

bool is_entity_in_collision_exceptions(Entity entity, Collider2DComponent* collider2DComponent);

SESpatialHashMap* globalSpatialHashMap = NULL;

CollisionResult cre_collision_process_entity_collisions(Entity entity) {
    Collider2DComponent* colliderComponent = component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
    CollisionResult collisionResult = { .sourceEntity = entity, .collidedEntityCount = 0 };
    SESpatialHashMapCollisionResult hashMapCollisionResult = se_spatial_hash_map_compute_collision(globalSpatialHashMap, entity);
    for (size_t i = 0; i < hashMapCollisionResult.collisionCount; i++) {
        if (!is_entity_in_collision_exceptions(hashMapCollisionResult.collisions[i], colliderComponent)) {
            SE_ASSERT_FMT(collisionResult.collidedEntityCount < CRE_MAX_ENTITY_COLLISION, "Collisions for entity '%d' beyond the limit of %d.  Consider increasing 'CRE_MAX_ENTITY_COLLISION'!", entity, CRE_MAX_ENTITY_COLLISION);
            collisionResult.collidedEntities[collisionResult.collidedEntityCount++] = hashMapCollisionResult.collisions[i];
        }
    }
    return collisionResult;
}

CollisionResult cre_collision_process_mouse_collisions(SERect2* collisionRect) {
    CollisionResult collisionResult = { .sourceEntity = NULL_ENTITY, .collidedEntityCount = 0 };
    const EntitySystem* collisionSystem = collision_ec_system_get();
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const Entity otherEntity = collisionSystem->entities[i];
        Transform2DComponent* otherTransformComponent = component_manager_get_component(otherEntity, ComponentDataIndex_TRANSFORM_2D);
        Collider2DComponent* otherColliderComponent = component_manager_get_component(otherEntity, ComponentDataIndex_COLLIDER_2D);
        SERect2 otherCollisionRect = cre_get_collision_rectangle(otherEntity, otherTransformComponent, otherColliderComponent);
        if (se_rect2_does_rectangles_overlap(collisionRect, &otherCollisionRect)) {
            collisionResult.collidedEntities[collisionResult.collidedEntityCount++] = otherEntity;
            if (collisionResult.collidedEntityCount >= CRE_MAX_ENTITY_COLLISION) {
                se_logger_warn("Reached collided entity limit of '%d' (with mouse)", CRE_MAX_ENTITY_COLLISION);
                break;
            }
        }
    }
    return collisionResult;
}

void cre_collision_set_global_spatial_hash_map(SESpatialHashMap* hashMap) {
    globalSpatialHashMap = hashMap;
}

SESpatialHashMap* cre_collision_get_global_spatial_hash_map() {
    return globalSpatialHashMap;
}

// Internal functions
bool is_entity_in_collision_exceptions(Entity entity, Collider2DComponent* collider2DComponent) {
    for (size_t i = 0; i < collider2DComponent->collisionExceptionCount; i++) {
        if (entity == collider2DComponent->collisionExceptions[i]) {
            return true;
        }
    }
    return false;
}

SERect2 cre_get_collision_rectangle(Entity entity, Transform2DComponent* transform2DComponent, Collider2DComponent* collider2DComponent) {
    const SETransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity,
            transform2DComponent);
    SERect2 collisionRect = {
        .x = globalTransform->position.x,
        .y = globalTransform->position.y,
        .w = globalTransform->scale.x * collider2DComponent->extents.w,
        .h = globalTransform->scale.y * collider2DComponent->extents.h,
    };
    if (collisionRect.w < 0.0f) {
        collisionRect.x += collisionRect.w;
        collisionRect.w = fabsf(collisionRect.w);
    }
    if (collisionRect.h < 0.0f) {
        collisionRect.y += collisionRect.h;
        collisionRect.h = fabsf(collisionRect.h);
    }
    return collisionRect;
}
