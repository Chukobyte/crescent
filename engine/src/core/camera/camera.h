#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/ecs/entity.h>
#include <seika/event.h>
#include <seika/math/math.h>

#include "../ecs/components/transform2d_component.h"

typedef enum CreCameraMode {
    CreCameraMode_MANUAL = 0,
    CreCameraMode_FOLLOW_ENTITY = 1,
} CreCameraMode;

typedef enum CreCameraArchorMode {
    CreCameraArchorMode_FIXED_TOP_LEFT = 0,
    CreCameraArchorMode_DRAG_CENTER = 1,
} CreCameraArchorMode;

typedef struct CRECamera2D {
    SkaRect2 boundary;
    SkaVector2 viewport;
    SkaVector2 offset;
    SkaVector2 zoom;
    CreCameraMode mode;
    CreCameraArchorMode archorMode;
    SkaEntity entityFollowing;
    SkaObserver onEntityTransformChangeObserver;
    SkaObserver onEntityExitSceneObserver;
} CRECamera2D;

void cre_camera2d_clamp_viewport_to_boundary(CRECamera2D* camera2D);
void cre_camera2d_follow_entity(CRECamera2D* camera2D, SkaEntity entity);
void cre_camera2d_unfollow_entity(CRECamera2D* camera2D, SkaEntity entity);
void cre_camera2d_update_entity_follow(CRECamera2D* camera2D, Transform2DComponent* transform2DComponent);
void cre_camera2d_calculate_view_matrix(CRECamera2D* camera2D, mat4 viewMatrix);

#ifdef __cplusplus
}
#endif
