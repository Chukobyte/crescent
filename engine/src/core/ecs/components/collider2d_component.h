#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <seika/ecs/entity.h>
#include <seika/math/math.h>

typedef struct Collider2DComponent {
    SkaSize2D extents; // TODO: Default to a box, implement other colliders later.  Also should use extents instead of rect!
    SkaEntity collisionExceptions[4];
    size_t collisionExceptionCount;
    SkaColor color; // For debug purposes
} Collider2DComponent;

Collider2DComponent* collider2d_component_create();
void collider2d_component_delete(Collider2DComponent* collider2DComponent);
Collider2DComponent* collider2d_component_copy(const Collider2DComponent* collider2DComponent);

#ifdef __cplusplus
}
#endif
