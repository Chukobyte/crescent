#include "collider2d_component.h"

#include "../../memory/rbe_mem.h"

Collider2DComponent* collider2d_component_create() {
    Collider2DComponent* collider2DComponent = RBE_MEM_ALLOCATE(Collider2DComponent);
    collider2DComponent->extents.w = 0.0f;
    collider2DComponent->extents.h = 0.0f;
    collider2DComponent->collisionExceptionCount = 0;
    collider2DComponent->color.r = 95.0f / 255.0f;
    collider2DComponent->color.g = 205.0f / 255.0f;
    collider2DComponent->color.b = 228.0f / 255.0f;
    collider2DComponent->color.a = 0.75f;
    return collider2DComponent;
}
