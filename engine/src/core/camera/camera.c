#include "camera.h"

#include "../game_properties.h"
#include "../ecs/component/node_component.h"
#include "../ecs/component/component.h"
#include "../ecs/component/transform2d_component.h"

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

void cre_camera2d_follow_entity(CRECamera2D* camera2D, Entity entity) {
    if (camera2D->entityFollowing == entity) {
        return;
    }
    cre_camera2d_unfollow_entity(camera2D, camera2D->entityFollowing);

    camera2D->entityFollowing = entity;
    camera2D->mode = CreCameraMode_FOLLOW_ENTITY;
    // Register to entity subjects
    NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component_unsafe(entity, ComponentDataIndex_NODE);
    if (nodeComponent != NULL) {
        se_subject_register_observer(&nodeComponent->onSceneTreeExit, &camera2D->onEntityExitSceneObserver);
    }
    Transform2DComponent* transform2DComponent = (Transform2DComponent *) component_manager_get_component_unsafe(entity, ComponentDataIndex_TRANSFORM_2D);
    if (transform2DComponent != NULL) {
        se_subject_register_observer(&transform2DComponent->onTransformChanged, &camera2D->onEntityTransformChangeObserver);
        // Trigger update right away so camera can be in position
        // TODO: Call the function this notifies instead of triggering
        se_subject_notify_observers(&transform2DComponent->onTransformChanged, &(SESubjectNotifyPayload) {
            .data = &(ComponentEntityUpdatePayload) { .entity = entity, .component = transform2DComponent, .componentType = ComponentType_TRANSFORM_2D },
            .type = 0
        });
    }
}

void cre_camera2d_unfollow_entity(CRECamera2D* camera2D, Entity entity) {
    if (entity != NULL_ENTITY && entity == camera2D->entityFollowing) {
        camera2D->entityFollowing = NULL_ENTITY;
        camera2D->mode = CreCameraMode_MANUAL;
        // Unregister from entity subjects
        NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component_unsafe(entity, ComponentDataIndex_NODE);
        if (nodeComponent != NULL) {
            se_subject_unregister_observer(&nodeComponent->onSceneTreeExit, &camera2D->onEntityExitSceneObserver);
        }
        Transform2DComponent* transform2DComponent = (Transform2DComponent *) component_manager_get_component_unsafe(entity, ComponentDataIndex_TRANSFORM_2D);
        if (transform2DComponent != NULL) {
            se_subject_unregister_observer(&transform2DComponent->onTransformChanged, &camera2D->onEntityTransformChangeObserver);
        }
    }
}
