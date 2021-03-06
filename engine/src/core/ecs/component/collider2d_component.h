#pragma once

#include <stddef.h>
#include "../entity/entity.h"
#include "../../math/rbe_math.h"

typedef struct Collider2DComponent {
    Size2D extents; // TODO: Default to a box, implement other colliders later.  Also should use extents instead of rect!
    Entity collisionExceptions[4];
    size_t collisionExceptionCount;
    Color color; // For debug purposes
} Collider2DComponent;

Collider2DComponent* collider2d_component_create();
