#include "collider2d_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

Collider2DComponent* collider2d_component_create() {
    Collider2DComponent* collider2DComponent = SE_MEM_ALLOCATE(Collider2DComponent);
    collider2DComponent->extents.w = 0.0f;
    collider2DComponent->extents.h = 0.0f;
    collider2DComponent->collisionExceptionCount = 0;
    collider2DComponent->color.r = 95.0f / 255.0f;
    collider2DComponent->color.g = 205.0f / 255.0f;
    collider2DComponent->color.b = 228.0f / 255.0f;
    collider2DComponent->color.a = 0.75f;
    return collider2DComponent;
}

void collider2d_component_delete(Collider2DComponent* collider2DComponent) {
    SE_MEM_FREE(collider2DComponent);
}

Collider2DComponent* collider2d_component_copy(const Collider2DComponent* collider2DComponent) {
    Collider2DComponent* copiedNode = SE_MEM_ALLOCATE(Collider2DComponent);
    memcpy(copiedNode, collider2DComponent, sizeof(Collider2DComponent));
    return copiedNode;
}
