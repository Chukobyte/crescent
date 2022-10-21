#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../seika/src/math/se_math.h"

typedef struct CRECamera2D {
    Rect2 boundary;
    Vector2 viewport;
    Vector2 offset;
    Vector2 zoom;
} CRECamera2D;

void cre_camera2d_clamp_viewport_to_boundary(CRECamera2D* camera2D);

#ifdef __cplusplus
}
#endif
