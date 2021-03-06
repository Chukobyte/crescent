#pragma once

#include "../math/rbe_math.h"

typedef struct RBECamera2D {
    Rect2 boundary;
    Vector2 viewport;
    Vector2 offset;
    Vector2 zoom;
} RBECamera2D;

void rbe_camera2d_clamp_viewport_to_boundary(RBECamera2D* camera2D);
