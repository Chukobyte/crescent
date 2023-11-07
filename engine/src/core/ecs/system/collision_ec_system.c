#include "collision_ec_system.h"

#include "../seika/src/rendering/renderer.h"
#include "../seika/src/utils/se_string_util.h"
#include "../seika/src/utils/se_assert.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/collider2d_component.h"
#include "../../physics/collision/collision.h"
#include "../../scene/scene_manager.h"
#include "../../game_properties.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_utils.h"

CreEntitySystem* collisionSystem = NULL;
SETexture* collisionOutlineTexture = NULL;
SERect2 colliderDrawSource = { .x=0.0f, .y=0.0f, .w=1.0f, .h=1.0f };

void collision_system_entity_unregistered(CreEntity entity);
void collision_system_fixed_update(float deltaTime);
void collision_system_render();

void collision_system_on_node_entered_scene(CreEntity entity);

void collision_system_on_transform_update(SESubjectNotifyPayload* payload);

SEObserver collisionOnEntityTransformChangeObserver = { .on_notify = collision_system_on_transform_update };
SESpatialHashMap* spatialHashMap = NULL;

CreEntitySystem* cre_collision_ec_system_create() {
//    SE_ASSERT(collisionSystem == NULL);
    collisionSystem = cre_ec_system_create();
    collisionSystem->name = se_strdup("Collision");
    collisionSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_COLLIDER_2D;

    collisionSystem->on_entity_entered_scene_func = collision_system_on_node_entered_scene;
    collisionSystem->on_entity_unregistered_func = collision_system_entity_unregistered;
    collisionSystem->fixed_update_func = collision_system_fixed_update;

    CREGameProperties* gameProps = cre_game_props_get();
    SE_ASSERT(cre_game_props_get() != NULL);
    if (gameProps->areCollidersVisible) {
        collisionSystem->render_func = collision_system_render;
        collisionOutlineTexture = se_texture_create_solid_colored_texture(1, 1, 255);
        SE_ASSERT(collisionOutlineTexture != NULL);
    }

    // One time init of global spatial hash map
    static bool isGlobalSpatialHashMapInitialized = false;
    if (!isGlobalSpatialHashMapInitialized) {
        const int initialCellSize = 64;
        spatialHashMap = se_spatial_hash_map_create(initialCellSize);
        cre_collision_set_global_spatial_hash_map(spatialHashMap);
        isGlobalSpatialHashMapInitialized = true;
    }

    return collisionSystem;
}

CreEntitySystem* cre_collision_ec_system_get() {
    return collisionSystem;
}

void collision_system_entity_unregistered(CreEntity entity) {
    se_spatial_hash_map_remove(spatialHashMap, entity);
    // Register to entity's 'on transform changed' event
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                          CreComponentDataIndex_TRANSFORM_2D);
    SE_ASSERT(transformComp != NULL);
    se_event_unregister_observer(&transformComp->onTransformChanged, &collisionOnEntityTransformChangeObserver);
}

// TODO: Temp, figure out how we want to handle caching the global transform
void collision_system_fixed_update(float deltaTime) {
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const CreEntity entity = collisionSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        transformComp->isGlobalTransformDirty = true;
    }
}

void collision_system_render() {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const CreEntity entity = collisionSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        const Collider2DComponent* colliderComp = (Collider2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLLIDER_2D);
        const CRECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        SETransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transformComp);
        static SEVector2 origin = { 0.0f, 0.0f };
        cre_scene_utils_apply_camera_and_origin_translation(globalTransform, &origin, transformComp->ignoreCamera);
        transformComp->isGlobalTransformDirty = true; // TODO: Make global transform const
        const SESize2D colliderDrawSize = {
            colliderComp->extents.w * renderCamera->zoom.x,
            colliderComp->extents.h * renderCamera->zoom.y
        };
        se_renderer_queue_sprite_draw_call(
            collisionOutlineTexture,
            colliderDrawSource,
            colliderDrawSize,
            colliderComp->color,
            false,
            false,
            globalTransform,
            SE_RENDERER_MAX_Z_INDEX, // Use the max possible z index value to draw colliders on top of other things...
            NULL
        );
    }
}

void collision_system_on_node_entered_scene(CreEntity entity) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component_unchecked(entity,
                                          CreComponentDataIndex_TRANSFORM_2D);
    Collider2DComponent* colliderComp = (Collider2DComponent*) cre_component_manager_get_component_unchecked(entity,
                                        CreComponentDataIndex_COLLIDER_2D);
    if (transformComp != NULL && colliderComp != NULL) {
        SERect2 collisionRect = cre_get_collision_rectangle(entity, transformComp, colliderComp);
        se_spatial_hash_map_insert_or_update(spatialHashMap, entity, &collisionRect);
        // Register to entity's 'on transform changed' event
        se_event_register_observer(&transformComp->onTransformChanged, &collisionOnEntityTransformChangeObserver);
    }
}

void collision_system_on_transform_update(SESubjectNotifyPayload* payload) {
    CreComponentEntityUpdatePayload* updatePayload = (CreComponentEntityUpdatePayload*) payload->data;
    Transform2DComponent* transformComp = (Transform2DComponent*) updatePayload->component;
    const CreEntity entity = updatePayload->entity;

    Collider2DComponent* colliderComp = (Collider2DComponent*) cre_component_manager_get_component_unchecked(entity,
                                        CreComponentDataIndex_COLLIDER_2D);
    if (transformComp != NULL && colliderComp != NULL) {
        SERect2 collisionRect = cre_get_collision_rectangle(entity, transformComp, colliderComp);
        se_spatial_hash_map_insert_or_update(spatialHashMap, entity, &collisionRect);
    }
}
