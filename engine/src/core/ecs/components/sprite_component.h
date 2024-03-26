#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <seika/rendering/texture.h>
#include <seika/rendering/shader/shader_instance_minimal.h>
#include <seika/math/math.h>

typedef struct SpriteComponent {
    SkaTexture* texture;
    SkaRect2 drawSource;
    SkaVector2 origin;
    bool flipH;
    bool flipV;
    SkaColor modulate;
    SkaShaderInstanceId shaderInstanceId;
} SpriteComponent;

SpriteComponent* sprite_component_create();
void sprite_component_delete(SpriteComponent* spriteComponent);
SpriteComponent* sprite_component_copy(const SpriteComponent* spriteComponent);

#ifdef __cplusplus
}
#endif
