#pragma once

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
