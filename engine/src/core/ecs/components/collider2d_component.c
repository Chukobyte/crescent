#include "collider2d_component.h"

#include <string.h>

#include <seika/memory.h>

#define COLLIDER_2D_COMPONENT_COLOR SKA_STRUCT_LITERAL(SkaColor){ 95.0f / 255.0f, 205.0f / 255.0f, 228.0f / 255.0f, 0.75f }

Collider2DComponent* collider2d_component_create() {
    Collider2DComponent* collider2DComponent = SKA_ALLOC_ZEROED(Collider2DComponent);
    collider2DComponent->color = COLLIDER_2D_COMPONENT_COLOR;
    return collider2DComponent;
}

void collider2d_component_delete(Collider2DComponent* collider2DComponent) {
    SKA_FREE(collider2DComponent);
}

Collider2DComponent* collider2d_component_copy(const Collider2DComponent* collider2DComponent) {
    Collider2DComponent* copiedNode = SKA_ALLOC(Collider2DComponent);
    memcpy(copiedNode, collider2DComponent, sizeof(Collider2DComponent));
    return copiedNode;
}
