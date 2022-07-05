#include "camera_manager.h"

#include "camera.h"

static const RBECamera2D DEFAULT_CAMERA =   {
    .boundary = {
        .x = -10000000.0f,
        .y = -10000000.0f,
        .w = 10000000.0f,
        .h = 10000000.0f
    },
    .viewport = { .x = 0.0f, .y = 0.0f },
    .zoom = { .x = 1.0f, .y = 1.0f },
    .offset = { .x = 0.0f, .y = 0.0f }

};

static RBECamera2D currentCamera = {
    .boundary = {
        .x = -10000000.0f,
        .y = -10000000.0f,
        .w = 10000000.0f,
        .h = 10000000.0f
    },
    .viewport = { .x = 0.0f, .y = 0.0f },
    .zoom = { .x = 1.0f, .y = 1.0f },
    .offset = { .x = 0.0f, .y = 0.0f }

};

RBECamera2D* rbe_camera_manager_get_current_camera() {
    return &currentCamera;
}

RBECamera2D* rbe_camera_manager_get_default_camera() {
    return &DEFAULT_CAMERA;
}

void rbe_camera_manager_reset_current_camera() {
    currentCamera = DEFAULT_CAMERA;
}
