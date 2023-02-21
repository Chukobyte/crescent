#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "../seika/src/rendering/texture.h"
#include "../seika/src/rendering/shader/shader_instance_minimal.h"
#include "../seika/src/math/se_math.h"

typedef struct SpriteComponent {
    Texture* texture;
    Rect2 drawSource;
    Vector2 origin;
    bool flipX;
    bool flipY;
    Color modulate;
    ShaderInstanceId shaderInstanceId;
} SpriteComponent;

SpriteComponent* sprite_component_create();
void sprite_component_delete(SpriteComponent* spriteComponent);
SpriteComponent* sprite_component_copy(const SpriteComponent* spriteComponent);

#ifdef __cplusplus
}
#endif
