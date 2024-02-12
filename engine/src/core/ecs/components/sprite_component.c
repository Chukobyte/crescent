#include "sprite_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

SpriteComponent* sprite_component_create() {
    SpriteComponent* spriteComponent = SE_MEM_ALLOCATE(SpriteComponent);
    spriteComponent->flipH = false;
    spriteComponent->flipV = false;
    spriteComponent->modulate = SKA_COLOR_WHITE;
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
