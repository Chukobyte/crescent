#include "camera.h"

#include "../game_properties.h"
#include "../ecs/component/node_component.h"
#include "../ecs/component/component.h"
#include "../scene/scene_manager.h"

void cre_camera2d_clamp_viewport_to_boundary(CRECamera2D* camera2D) {
    CREGameProperties* gameProperties = cre_game_props_get();
    if (camera2D->viewport.x < camera2D->boundary.x) {
        camera2D->viewport.x = camera2D->boundary.x;
    } else if (camera2D->viewport.x > camera2D->boundary.w - (float) gameProperties->resolutionWidth) {
        camera2D->viewport.x = camera2D->boundary.w - (float) gameProperties->resolutionWidth;
    }
    if (camera2D->viewport.y < camera2D->boundary.y) {
        camera2D->viewport.y = camera2D->boundary.y;
    } else if (camera2D->viewport.y > camera2D->boundary.h - (float) gameProperties->resolutionHeight) {
        camera2D->viewport.y = camera2D->boundary.h - (float) gameProperties->resolutionHeight;
    }
}

void cre_camera2d_follow_entity(CRECamera2D* camera2D, CreEntity entity) {
    if (camera2D->entityFollowing == entity) {
        return;
    }
    cre_camera2d_unfollow_entity(camera2D, camera2D->entityFollowing);

    camera2D->entityFollowing = entity;
    camera2D->mode = CreCameraMode_FOLLOW_ENTITY;
    // Register to entity events
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component_unchecked(entity,
                                   CreComponentDataIndex_NODE);
    if (nodeComponent != NULL) {
        se_event_register_observer(&nodeComponent->onSceneTreeExit, &camera2D->onEntityExitSceneObserver);
    }
    Transform2DComponent* transform2DComponent = (Transform2DComponent *) cre_component_manager_get_component_unchecked(
                entity, CreComponentDataIndex_TRANSFORM_2D);
    if (transform2DComponent != NULL) {
        se_event_register_observer(&transform2DComponent->onTransformChanged, &camera2D->onEntityTransformChangeObserver);
        // Trigger update right away so camera can be in position
        cre_camera2d_update_entity_follow(camera2D, transform2DComponent);
    }
}

void cre_camera2d_unfollow_entity(CRECamera2D* camera2D, CreEntity entity) {
    if (entity != CRE_NULL_ENTITY && entity == camera2D->entityFollowing) {
        camera2D->entityFollowing = CRE_NULL_ENTITY;
        camera2D->mode = CreCameraMode_MANUAL;
        // Unregister from entity events
        NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component_unchecked(entity,
                                       CreComponentDataIndex_NODE);
        if (nodeComponent != NULL) {
            se_event_unregister_observer(&nodeComponent->onSceneTreeExit, &camera2D->onEntityExitSceneObserver);
        }
        Transform2DComponent* transform2DComponent = (Transform2DComponent *) cre_component_manager_get_component_unchecked(
                    entity, CreComponentDataIndex_TRANSFORM_2D);
        if (transform2DComponent != NULL) {
            se_event_unregister_observer(&transform2DComponent->onTransformChanged,
                                         &camera2D->onEntityTransformChangeObserver);
        }
    }
}

void cre_camera2d_update_entity_follow(CRECamera2D* camera2D, Transform2DComponent* transform2DComponent) {
    SETransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(camera2D->entityFollowing, transform2DComponent);
    CREGameProperties* gameProperties = cre_game_props_get();
    // TODO: Check for mode
    SEVector2 newCameraPos = {
        globalTransform->position.x - ((float) gameProperties->resolutionWidth / 2.0f),
        globalTransform->position.y - ((float) gameProperties->resolutionHeight / 2.0f)
    };
    camera2D->viewport = newCameraPos;
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
}
