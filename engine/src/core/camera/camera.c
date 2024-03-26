#include "camera.h"

#include "../game_properties.h"
#include "../ecs/ecs_globals.h"
#include "../ecs/components/node_component.h"
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

void cre_camera2d_follow_entity(CRECamera2D* camera2D, SkaEntity entity) {
    if (camera2D->entityFollowing == entity) {
        return;
    }
    cre_camera2d_unfollow_entity(camera2D, camera2D->entityFollowing);

    camera2D->entityFollowing = entity;
    camera2D->mode = CreCameraMode_FOLLOW_ENTITY;
    // Register to entity events
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component_unchecked(entity, NODE_COMPONENT_INDEX);
    if (nodeComponent != NULL) {
        ska_event_register_observer(&nodeComponent->onSceneTreeExit, &camera2D->onEntityExitSceneObserver);
    }
    Transform2DComponent* transform2DComponent = (Transform2DComponent *)ska_ecs_component_manager_get_component_unchecked(entity, TRANSFORM2D_COMPONENT_INDEX);
    if (transform2DComponent != NULL) {
        ska_event_register_observer(&transform2DComponent->onTransformChanged, &camera2D->onEntityTransformChangeObserver);
        // Trigger update right away so camera can be in position
        cre_camera2d_update_entity_follow(camera2D, transform2DComponent);
    }
}

void cre_camera2d_unfollow_entity(CRECamera2D* camera2D, SkaEntity entity) {
    if (entity != SKA_NULL_ENTITY && entity == camera2D->entityFollowing) {
        camera2D->entityFollowing = SKA_NULL_ENTITY;
        camera2D->mode = CreCameraMode_MANUAL;
        // Unregister from entity events
        NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component_unchecked(entity, NODE_COMPONENT_INDEX);
        if (nodeComponent != NULL) {
            ska_event_unregister_observer(&nodeComponent->onSceneTreeExit, &camera2D->onEntityExitSceneObserver);
        }
        Transform2DComponent* transform2DComponent = (Transform2DComponent*)ska_ecs_component_manager_get_component_unchecked(entity, TRANSFORM2D_COMPONENT_INDEX);
        if (transform2DComponent != NULL) {
            ska_event_unregister_observer(&transform2DComponent->onTransformChanged,&camera2D->onEntityTransformChangeObserver);
        }
    }
}

void cre_camera2d_update_entity_follow(CRECamera2D* camera2D, Transform2DComponent* transform2DComponent) {
    SkaTransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(camera2D->entityFollowing, transform2DComponent);
    CREGameProperties* gameProperties = cre_game_props_get();
    // TODO: Check for mode
    SkaVector2 newCameraPos = {
        globalTransform->position.x - ((float) gameProperties->resolutionWidth / 2.0f),
        globalTransform->position.y - ((float) gameProperties->resolutionHeight / 2.0f)
    };
    camera2D->viewport = newCameraPos;
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
}

void cre_camera2d_calculate_view_matrix(CRECamera2D* camera2D, mat4 viewMatrix) {
    glm_mat4_identity(viewMatrix);
    glm_scale(viewMatrix, (vec3){ 1.0f / camera2D->zoom.x, 1.0f / camera2D->zoom.y, 0.0f });
    // Skip rotation
    glm_translate(viewMatrix, (vec3){ -camera2D->viewport.x, -camera2D->viewport.y, 0.0f });
}
