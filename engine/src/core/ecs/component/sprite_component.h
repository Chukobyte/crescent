#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "../../rendering/texture.h"
#include "../../math/rbe_math.h"

typedef struct SpriteComponent {
    Texture* texture;
    Rect2 drawSource;
    Vector2 origin;
    bool flipX;
    bool flipY;
    Color modulate;
} SpriteComponent;

SpriteComponent* sprite_component_create();
void sprite_component_delete(SpriteComponent* spriteComponent);
SpriteComponent* sprite_component_copy(const SpriteComponent* spriteComponent);

#ifdef __cplusplus
}
#endif
