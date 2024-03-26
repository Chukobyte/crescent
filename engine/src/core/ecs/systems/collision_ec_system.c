#include "collision_ec_system.h"

#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/string.h>
#include <seika/assert.h>

#include "../ecs_globals.h"

#include "../components/transform2d_component.h"
#include "../components/collider2d_component.h"
#include "../../physics/collision/collision.h"
#include "../../scene/scene_manager.h"
#include "../../game_properties.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

static void collision_system_on_transform_update(SkaSubjectNotifyPayload* payload);

SkaECSSystem* collisionSystem = NULL;
SkaTexture* collisionOutlineTexture = NULL;
SkaRect2 colliderDrawSource = { .x=0.0f, .y=0.0f, .w=1.0f, .h=1.0f };

SkaObserver collisionOnEntityTransformChangeObserver = { .on_notify = collision_system_on_transform_update };
SkaSpatialHashMap* spatialHashMap = NULL;

static void on_ec_system_registered(SkaECSSystem* system);
static void on_ec_system_destroyed(SkaECSSystem* system);
static void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity);
static void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity);
static void fixed_update(SkaECSSystem* system, f32 deltaTime);
static void collision_render(SkaECSSystem* system);

void cre_collision_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Collision");
    systemTemplate.on_ec_system_register = on_ec_system_registered;
    systemTemplate.on_ec_system_destroy = on_ec_system_destroyed;
    systemTemplate.on_entity_unregistered_func = on_entity_unregistered;
    systemTemplate.on_entity_entered_scene_func = on_entity_entered_scene;
    systemTemplate.fixed_update_func = fixed_update;
    systemTemplate.render_func = collision_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, Collider2DComponent);
}

void on_ec_system_registered(SkaECSSystem* system) {
    collisionSystem = system;
    CREGameProperties* gameProps = cre_game_props_get();
    SKA_ASSERT(cre_game_props_get() != NULL);
    if (gameProps->areCollidersVisible) {
        system->render_func = collision_render;
        collisionOutlineTexture = ska_texture_create_solid_colored_texture(1, 1, 255);
        SKA_ASSERT(collisionOutlineTexture != NULL);
    }

    static const int32 initialCellSize = 64;
    spatialHashMap = ska_spatial_hash_map_create(initialCellSize);
    cre_collision_set_global_spatial_hash_map(spatialHashMap);
}

void on_ec_system_destroyed(SkaECSSystem* system) {
    ska_spatial_hash_map_destroy(spatialHashMap);
    spatialHashMap = NULL;
    collisionSystem = NULL;
}

void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity) {
    ska_spatial_hash_map_remove(spatialHashMap, entity);
    // Register to entity's 'on transform changed' event
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity,TRANSFORM2D_COMPONENT_INDEX);
    SKA_ASSERT(transformComp != NULL);
    ska_event_unregister_observer(&transformComp->onTransformChanged, &collisionOnEntityTransformChangeObserver);
}

void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity) {
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    Collider2DComponent* colliderComp = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity, COLLIDER2D_COMPONENT_INDEX);
    if (transformComp != NULL && colliderComp != NULL) {
        SkaRect2 collisionRect = cre_get_collision_rectangle(entity, transformComp, colliderComp);
        ska_spatial_hash_map_insert_or_update(spatialHashMap, entity, &collisionRect);
        // Register to entity's 'on transform changed' event
        ska_event_register_observer(&transformComp->onTransformChanged, &collisionOnEntityTransformChangeObserver);
    }
}

void fixed_update(SkaECSSystem* system, f32 deltaTime) {
    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity,TRANSFORM2D_COMPONENT_INDEX);
        transformComp->isGlobalTransformDirty = true;
    }
}

void collision_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity,TRANSFORM2D_COMPONENT_INDEX);
        const Collider2DComponent* colliderComp = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity,COLLIDER2D_COMPONENT_INDEX);
        const CRECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, transformComp, &SKA_VECTOR2_ZERO);
        const SkaSize2D colliderDrawSize = {
            .w = colliderComp->extents.w * renderCamera->zoom.x,
            .h = colliderComp->extents.h * renderCamera->zoom.y
        };

        ska_renderer_queue_sprite_draw(
            collisionOutlineTexture,
            colliderDrawSource,
            colliderDrawSize,
            colliderComp->color,
            false,
            false,
            &renderResource.transform2D,
            SKA_RENDERER_MAX_Z_INDEX, // Use the max possible z index value to draw colliders on top of other things...
            NULL
        );
    }
}

void collision_system_on_transform_update(SkaSubjectNotifyPayload* payload) {
    CreComponentEntityUpdatePayload* updatePayload = (CreComponentEntityUpdatePayload*) payload->data;
    Transform2DComponent* transformComp = (Transform2DComponent*) updatePayload->component;
    const SkaEntity entity = updatePayload->entity;

    Collider2DComponent* colliderComp = (Collider2DComponent*)ska_ecs_component_manager_get_component_unchecked(entity, COLLIDER2D_COMPONENT_INDEX);
    if (transformComp != NULL && colliderComp != NULL) {
        SkaRect2 collisionRect = cre_get_collision_rectangle(entity, transformComp, colliderComp);
        ska_spatial_hash_map_insert_or_update(spatialHashMap, entity, &collisionRect);
    }
}

SkaECSSystem* cre_collision_ec_system_get() {
    return collisionSystem;
}
