#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/utils/observer.h>
#include <seika/math/se_math.h>

#include "../ecs/entity/entity.h"
#include "../ecs/component/transform2d_component.h"

typedef enum CreCameraMode {
    CreCameraMode_MANUAL = 0,
    CreCameraMode_FOLLOW_ENTITY = 1,
} CreCameraMode;

typedef enum CreCameraArchorMode {
    CreCameraArchorMode_FIXED_TOP_LEFT = 0,
    CreCameraArchorMode_DRAG_CENTER = 1,
} CreCameraArchorMode;

typedef struct CRECamera2D {
    SERect2 boundary;
    SEVector2 viewport;
    SEVector2 offset;
    SEVector2 zoom;
    CreCameraMode mode;
    CreCameraArchorMode archorMode;
    CreEntity entityFollowing;
    SEObserver onEntityTransformChangeObserver;
    SEObserver onEntityExitSceneObserver;
} CRECamera2D;

void cre_camera2d_clamp_viewport_to_boundary(CRECamera2D* camera2D);
void cre_camera2d_follow_entity(CRECamera2D* camera2D, CreEntity entity);
void cre_camera2d_unfollow_entity(CRECamera2D* camera2D, CreEntity entity);
void cre_camera2d_update_entity_follow(CRECamera2D* camera2D, Transform2DComponent* transform2DComponent);

#ifdef __cplusplus
}
#endif
