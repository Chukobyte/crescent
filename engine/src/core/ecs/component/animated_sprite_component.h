#pragma once

#include <stdint.h>

#include "../../rendering/animation.h"
#include "../../data_structures/rbe_hash_map_string.h"

typedef struct AnimatedSpriteComponent {
    RBEStringHashMap* animations;
    Animation* currentAnimation;
    bool isPlaying;
    bool doesLoop;
    bool flipX;
    bool flipY;
    uint32_t startAnimationTickTime;
} AnimatedSpriteComponent;

AnimatedSpriteComponent* animated_sprite_component_create();
void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, Animation* animation);
