#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../../animation/animation.h"
#include "../../math/rbe_math.h"
#include "../../data_structures/rbe_hash_map_string.h"

#define ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS 16

typedef struct AnimatedSpriteComponent {
    Animation animations[ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS];
    Animation currentAnimation;
    size_t animationCount;
    Color modulate;
    bool isPlaying;
    Vector2 origin;
    bool flipX;
    bool flipY;
    uint32_t startAnimationTickTime;
} AnimatedSpriteComponent;

typedef struct AnimationQueryResult {
    Animation animation;
    bool success;
} AnimationQueryResult;

AnimatedSpriteComponent* animated_sprite_component_create();
void animated_sprite_component_delete(AnimatedSpriteComponent* animatedSpriteComponent);
AnimatedSpriteComponent* animated_sprite_component_copy(const AnimatedSpriteComponent* animatedSpriteComponent);
void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, Animation animation);
AnimationQueryResult animated_sprite_component_get_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);
bool animated_sprite_component_set_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);
