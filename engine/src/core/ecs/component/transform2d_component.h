#pragma once

#include <stdbool.h>

#include "../../math/rbe_math.h"

typedef struct Transform2DComponent {
    Vector2 position;
    Vector2 scale;
    float rotation; // Degrees
    int zIndex;
    bool isZIndexRelativeToParent;
    bool ignoreCamera;
} Transform2DComponent;

Transform2DComponent* transform2d_component_create();
