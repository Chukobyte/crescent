#include "sprite_component.h"

#include <string.h>

#include <seika/memory.h>

SpriteComponent* sprite_component_create() {
    SpriteComponent* spriteComponent = SKA_MEM_ALLOCATE(SpriteComponent);
    spriteComponent->flipH = false;
    spriteComponent->flipV = false;
    spriteComponent->modulate = SKA_COLOR_WHITE;
    spriteComponent->shaderInstanceId = SKA_SHADER_INSTANCE_INVALID_ID;

    return spriteComponent;
}

void sprite_component_delete(SpriteComponent* spriteComponent) {
    SKA_MEM_FREE(spriteComponent);
}

SpriteComponent* sprite_component_copy(const SpriteComponent* spriteComponent) {
    SpriteComponent* copiedNode = SKA_MEM_ALLOCATE(SpriteComponent);
    memcpy(copiedNode, spriteComponent, sizeof(SpriteComponent));
    return copiedNode;
}
