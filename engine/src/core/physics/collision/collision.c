#include "collision.h"

#include <seika/ecs/ecs.h>
#include <seika/utils/logger.h>
#include <seika/utils/se_assert.h>

#include "../../ecs/ecs_globals.h"
#include "../../ecs/system/ec_system.h"
#include "../../ecs/system/collision_ec_system.h"
#include "../../scene/scene_manager.h"

static bool is_entity_in_collision_exceptions(CreEntity entity, Collider2DComponent* collider2DComponent);

SESpatialHashMap* globalSpatialHashMap = NULL;

CollisionResult cre_collision_process_entity_collisions(CreEntity entity) {
    Collider2DComponent* colliderComponent = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity, COLLIDER2D_COMPONENT_INDEX);
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

CollisionResult cre_collision_process_mouse_collisions(const SKARect2* collisionRect) {
    CollisionResult collisionResult = { .sourceEntity = CRE_NULL_ENTITY, .collidedEntityCount = 0 };
    const SkaECSSystem* collisionSystem = cre_collision_ec_system_get();
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const CreEntity otherEntity = collisionSystem->entities[i];
        Transform2DComponent* otherTransformComponent = (Transform2DComponent*)ska_ecs_component_manager_get_component(otherEntity,TRANSFORM2D_COMPONENT_INDEX);
        Collider2DComponent* otherColliderComponent = (Collider2DComponent*)ska_ecs_component_manager_get_component(otherEntity,COLLIDER2D_COMPONENT_INDEX);
        SKARect2 otherCollisionRect = cre_get_collision_rectangle(otherEntity, otherTransformComponent, otherColliderComponent);
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
bool is_entity_in_collision_exceptions(CreEntity entity, Collider2DComponent* collider2DComponent) {
    for (size_t i = 0; i < collider2DComponent->collisionExceptionCount; i++) {
        if (entity == collider2DComponent->collisionExceptions[i]) {
            return true;
        }
    }
    return false;
}

SKARect2 cre_get_collision_rectangle(CreEntity entity, Transform2DComponent* transform2DComponent, Collider2DComponent* collider2DComponent) {
    const SKATransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transform2DComponent);
    SKARect2 collisionRect = {
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
