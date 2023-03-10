#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../seika/src/rendering/shader/shader_instance_minimal.h"
#include "../seika/src/math/se_math.h"
#include "../seika/src/data_structures/se_hash_map_string.h"

#include "../../animation/animation.h"

#define ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS 16

typedef struct AnimatedSpriteComponent {
    CreAnimation animations[ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS];
    CreAnimation currentAnimation;
    size_t animationCount;
    SEColor modulate;
    bool isPlaying;
    SEVector2 origin;
    bool flipH;
    bool flipV;
    uint32_t startAnimationTickTime;
    SEShaderInstanceId shaderInstanceId;
} AnimatedSpriteComponent;

typedef struct AnimationQueryResult {
    CreAnimation animation;
    bool success;
} AnimationQueryResult;

typedef struct AnimationFrameData {
    char texturePath[64];
    SERect2 drawSource;
    int frame;
} AnimationFrameData;

typedef struct AnimationData {
    AnimationFrameData animationFrames[CRE_MAX_ANIMATION_FRAMES];
    int frameCount;
    int currentFrame;
    int speed;
    char name[16];
    bool doesLoop;
    bool isValid;
} AnimationData;

typedef struct AnimatedSpriteComponentData {
    AnimationData animations[ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS];
    AnimationData currentAnimation;
    size_t animationCount;
    SEColor modulate;
    bool isPlaying;
    SEVector2 origin;
    bool flipH;
    bool flipV;
    uint32_t startAnimationTickTime;
} AnimatedSpriteComponentData;

AnimatedSpriteComponent* animated_sprite_component_create();
void animated_sprite_component_delete(AnimatedSpriteComponent* animatedSpriteComponent);
AnimatedSpriteComponent* animated_sprite_component_copy(const AnimatedSpriteComponent* animatedSpriteComponent);
// TODO: Validate if animation with name already exist
void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, CreAnimation animation);
AnimationQueryResult animated_sprite_component_get_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);
bool animated_sprite_component_set_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);

AnimatedSpriteComponentData* animated_sprite_component_data_create();
void animated_sprite_component_data_delete(AnimatedSpriteComponentData* animatedSpriteComponent);
AnimatedSpriteComponent* animated_sprite_component_data_copy_to_animated_sprite(const AnimatedSpriteComponentData* animatedSpriteComponent);
void animated_sprite_component_data_add_animation(AnimatedSpriteComponentData* animatedSpriteComponent, AnimationData animation);

#ifdef __cplusplus
}
#endif
