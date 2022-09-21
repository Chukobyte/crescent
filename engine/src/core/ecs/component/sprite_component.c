#include "sprite_component.h"

#include <string.h>

#include "../../memory/rbe_mem.h"

SpriteComponent* sprite_component_create() {
    SpriteComponent* spriteComponent = RBE_MEM_ALLOCATE(SpriteComponent);
    spriteComponent->texture = NULL;
    spriteComponent->drawSource.x = 0.0f;
    spriteComponent->drawSource.y = 0.0f;
    spriteComponent->drawSource.w = 0.0f;
    spriteComponent->drawSource.h = 0.0f;
    spriteComponent->origin.x = 0.0f;
    spriteComponent->origin.y = 0.0f;
    spriteComponent->flipX = false;
    spriteComponent->flipY = false;
    spriteComponent->modulate.r = 1.0f;
    spriteComponent->modulate.g = 1.0f;
    spriteComponent->modulate.b = 1.0f;
    spriteComponent->modulate.a = 1.0f;
    return spriteComponent;
}

void sprite_component_delete(SpriteComponent* spriteComponent) {
    RBE_MEM_FREE(spriteComponent);
}

SpriteComponent* sprite_component_copy(const SpriteComponent* spriteComponent) {
    SpriteComponent* copiedNode = RBE_MEM_ALLOCATE(SpriteComponent);
    memcpy(copiedNode, spriteComponent, sizeof(SpriteComponent));
    return copiedNode;
}
