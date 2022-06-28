#pragma once

#include "../../math/rbe_math.h"
#include "../entity/entity.h"

typedef struct Collider2DComponent {
    Rect2 rect; // TODO: Default to a box, implement other colliders later...
    Entity collisionExceptions[4];
    size_t collisionExceptionCount;
    Color color; // For debug purposes
} Collider2DComponent;

Collider2DComponent* collider2d_component_create();
