#include "camera_manager.h"

#include "camera.h"
#include "../ecs/component/transform2d_component.h"
#include "../ecs/component/component.h"
#include "../scene/scene_manager.h"
#include "../game_properties.h"

void camera2d_on_entity_transform_change(SESubjectNotifyPayload* payload);
void camera2d_on_entity_exit_scene(SESubjectNotifyPayload* payload);

static CRECamera2D DEFAULT_CAMERA =   {
    .boundary = {
        .x = -10000000.0f,
        .y = -10000000.0f,
        .w = 10000000.0f,
        .h = 10000000.0f
    },
    .viewport = { .x = 0.0f, .y = 0.0f },
    .zoom = { .x = 1.0f, .y = 1.0f },
    .offset = { .x = 0.0f, .y = 0.0f },
    .mode = CreCameraMode_MANUAL,
    .archorMode = CreCameraArchorMode_FIXED_TOP_LEFT,
    .entityFollowing = NULL_ENTITY,
    .onEntityTransformChangeObserver = { .on_notify = camera2d_on_entity_transform_change },
    .onEntityExitSceneObserver = { .on_notify = camera2d_on_entity_exit_scene }
};

static CRECamera2D currentCamera = {
    .boundary = {
        .x = -10000000.0f,
        .y = -10000000.0f,
        .w = 10000000.0f,
        .h = 10000000.0f
    },
    .viewport = { .x = 0.0f, .y = 0.0f },
    .zoom = { .x = 1.0f, .y = 1.0f },
    .offset = { .x = 0.0f, .y = 0.0f },
    .mode = CreCameraMode_MANUAL,
    .archorMode = CreCameraArchorMode_FIXED_TOP_LEFT,
    .entityFollowing = NULL_ENTITY,
    .onEntityTransformChangeObserver = { .on_notify = camera2d_on_entity_transform_change },
    .onEntityExitSceneObserver = { .on_notify = camera2d_on_entity_exit_scene }
};

CRECamera2D* cre_camera_manager_get_current_camera() {
    return &currentCamera;
}

CRECamera2D* cre_camera_manager_get_default_camera() {
    return &DEFAULT_CAMERA;
}

void cre_camera_manager_reset_current_camera() {
    currentCamera = DEFAULT_CAMERA;
}

// Observer callbacks
void camera2d_on_entity_transform_change(SESubjectNotifyPayload* payload) {
    ComponentEntityUpdatePayload* updatePayload = (ComponentEntityUpdatePayload*) payload->data;
    Transform2DComponent* transform2DComponent = (Transform2DComponent*) updatePayload->component;

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    cre_camera2d_update_entity_follow(camera2D, transform2DComponent);
}

void camera2d_on_entity_exit_scene(SESubjectNotifyPayload* payload) {
    const Entity entity = *(Entity*) payload->data;

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    cre_camera2d_unfollow_entity(camera2D, entity);
}
