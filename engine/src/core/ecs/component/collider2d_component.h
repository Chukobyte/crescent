#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "../seika/src/math/se_math.h"

#include "../entity/entity.h"

typedef struct Collider2DComponent {
    SESize2D extents; // TODO: Default to a box, implement other colliders later.  Also should use extents instead of rect!
    CreEntity collisionExceptions[4];
    size_t collisionExceptionCount;
    SEColor color; // For debug purposes
} Collider2DComponent;

Collider2DComponent* collider2d_component_create();
void collider2d_component_delete(Collider2DComponent* collider2DComponent);
Collider2DComponent* collider2d_component_copy(const Collider2DComponent* collider2DComponent);

#ifdef __cplusplus
}
#endif
