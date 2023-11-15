#include "sprite_component.h"

#include <string.h>

#include "seika/memory/se_mem.h"

SpriteComponent* sprite_component_create() {
    SpriteComponent* spriteComponent = SE_MEM_ALLOCATE(SpriteComponent);
    spriteComponent->texture = NULL;
    spriteComponent->drawSource.x = 0.0f;
    spriteComponent->drawSource.y = 0.0f;
    spriteComponent->drawSource.w = 0.0f;
    spriteComponent->drawSource.h = 0.0f;
    spriteComponent->origin.x = 0.0f;
    spriteComponent->origin.y = 0.0f;
    spriteComponent->flipH = false;
    spriteComponent->flipV = false;
    spriteComponent->modulate.r = 1.0f;
    spriteComponent->modulate.g = 1.0f;
    spriteComponent->modulate.b = 1.0f;
    spriteComponent->modulate.a = 1.0f;
    spriteComponent->shaderInstanceId = SE_SHADER_INSTANCE_INVALID_ID;

    return spriteComponent;
}

void sprite_component_delete(SpriteComponent* spriteComponent) {
    SE_MEM_FREE(spriteComponent);
}

SpriteComponent* sprite_component_copy(const SpriteComponent* spriteComponent) {
    SpriteComponent* copiedNode = SE_MEM_ALLOCATE(SpriteComponent);
    memcpy(copiedNode, spriteComponent, sizeof(SpriteComponent));
    return copiedNode;
}
