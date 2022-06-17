#include "animated_sprite_component.h"

#include <string.h>

#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"
#include "../../utils/logger.h"

#define RBE_MAX_ANIMATIONS 16

AnimatedSpriteComponent* animated_sprite_component_create() {
    AnimatedSpriteComponent* animatedSpriteComponent = RBE_MEM_ALLOCATE(AnimatedSpriteComponent);
    animatedSpriteComponent->animationCount = 0;
    animatedSpriteComponent->currentAnimation.isValid = false;
    animatedSpriteComponent->isPlaying = false;
    animatedSpriteComponent->doesLoop = true;
    animatedSpriteComponent->flipX = false;
    animatedSpriteComponent->flipY = false;

    return animatedSpriteComponent;
}

void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, Animation animation) {
    RBE_ASSERT_FMT(animatedSpriteComponent->animationCount + 1 < RBE_MAX_ANIMATIONS,
                   "Adding animation '%s' exceeds the max limit of animations which is '%d'", animation.name, RBE_MAX_ANIMATIONS);
    animatedSpriteComponent->animations[animatedSpriteComponent->animationCount++] = animation;
}

Animation animated_sprite_component_get_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name) {
    for (size_t i = 0; i < animatedSpriteComponent->animationCount; i++) {
        if (strcmp(animatedSpriteComponent->animations[i].name, name) == 0) {
            return animatedSpriteComponent->animations[i];
        }
    }
    RBE_ASSERT_FMT(false, "Failed to retrieve animation named '%s'", name);
    Animation invalidAnimation = {};
    return invalidAnimation;
}

Animation* animated_sprite_component_get_animation_ref(AnimatedSpriteComponent* animatedSpriteComponent, const char* name) {
    for (size_t i = 0; i < animatedSpriteComponent->animationCount; i++) {
        if (strcmp(animatedSpriteComponent->animations[i].name, name) == 0) {
            return &animatedSpriteComponent->animations[i];
        }
    }
    RBE_ASSERT_FMT(false, "Failed to retrieve animation named '%s'", name);
    return NULL;
}
