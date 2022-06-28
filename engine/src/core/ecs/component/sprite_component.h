#pragma once

#include <stdbool.h>

#include "../../math/rbe_math.h"
#include "../../rendering/texture.h"

typedef struct SpriteComponent {
    Texture* texture;
    Rect2 drawSource;
    bool flipX;
    bool flipY;
    Color modulate;
} SpriteComponent;

SpriteComponent* sprite_component_create();
