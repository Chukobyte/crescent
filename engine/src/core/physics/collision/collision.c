#include "collision.h"

#include "../../ecs/system/ec_system.h"
#include "../../ecs/system/collision_ec_system.h"
#include "../../ecs/component/transform2d_component.h"
#include "../../ecs/component/collider2d_component.h"
#include "../../utils/logger.h"
#include "../../scene/scene_manager.h"

// Temp collision stuff until a better solution is implemented...
bool is_entity_in_collision_exceptions(Entity entity, Collider2DComponent* collider2DComponent);
Rect2 get_collision_rectangle(Entity entity, Transform2DComponent* transform2DComponent, Collider2DComponent* collider2DComponent);
bool does_rectangles_collide(Rect2* sourceRect, Rect2* targetRect);

CollisionResult rbe_collision_process_entity_collisions(Entity entity) {
    CollisionResult collisionResult = { .sourceEntity = entity, .collidedEntityCount = 0 };
    const EntitySystem* collisionSystem = collision_ec_system_get();
    Transform2DComponent* transformComponent = component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
    Collider2DComponent* colliderComponent = component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
    Rect2 sourceCollisionRect = get_collision_rectangle(entity, transformComponent, colliderComponent);
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const Entity otherEntity = collisionSystem->entities[i];
        if (entity == otherEntity || is_entity_in_collision_exceptions(otherEntity, colliderComponent)) {
            continue;
        }
        Transform2DComponent* otherTransformComponent = component_manager_get_component(otherEntity, ComponentDataIndex_TRANSFORM_2D);
        Collider2DComponent* otherColliderComponent = component_manager_get_component(otherEntity, ComponentDataIndex_COLLIDER_2D);
        Rect2 otherCollisionRect = get_collision_rectangle(otherEntity, otherTransformComponent, otherColliderComponent);
        if (does_rectangles_collide(&sourceCollisionRect, &otherCollisionRect)) {
            collisionResult.collidedEntities[collisionResult.collidedEntityCount++] = otherEntity;
            if (collisionResult.collidedEntityCount >= RBE_MAX_ENTITY_COLLISION) {
                rbe_logger_warn("Reached collided entity limit of '%d'", RBE_MAX_ENTITY_COLLISION);
                break;
            }
        }
    }
    return collisionResult;
}

bool is_entity_in_collision_exceptions(Entity entity, Collider2DComponent* collider2DComponent) {
    for (size_t i = 0; i < collider2DComponent->collisionExceptionCount; i++) {
        if (entity == collider2DComponent->collisionExceptions[i]) {
            return true;
        }
    }
    return false;
}

Rect2 get_collision_rectangle(Entity entity, Transform2DComponent* transform2DComponent, Collider2DComponent* collider2DComponent) {
    const TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, transform2DComponent);
    Rect2 collisionRect = {
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

bool does_rectangles_collide(Rect2* sourceRect, Rect2* targetRect) {
    return (sourceRect->x + sourceRect->w >= targetRect->x) &&
           (targetRect->x + targetRect->w >= sourceRect->x) &&
           (sourceRect->y + sourceRect->h >= targetRect->y) &&
           (targetRect->y + targetRect->h >= sourceRect->y);
}
