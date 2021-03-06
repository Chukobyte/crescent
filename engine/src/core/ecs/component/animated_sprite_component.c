#include "animated_sprite_component.h"

#include <string.h>

#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"

#define RBE_MAX_ANIMATIONS 16

AnimatedSpriteComponent* animated_sprite_component_create() {
    AnimatedSpriteComponent* animatedSpriteComponent = RBE_MEM_ALLOCATE(AnimatedSpriteComponent);
    animatedSpriteComponent->animationCount = 0;
    animatedSpriteComponent->modulate.r = 1.0f;
    animatedSpriteComponent->modulate.g = 1.0f;
    animatedSpriteComponent->modulate.b = 1.0f;
    animatedSpriteComponent->modulate.a = 1.0f;
    animatedSpriteComponent->currentAnimation.isValid = false;
    animatedSpriteComponent->origin.x = 0.0f;
    animatedSpriteComponent->origin.y = 0.0f;
    animatedSpriteComponent->isPlaying = false;
    animatedSpriteComponent->flipX = false;
    animatedSpriteComponent->flipY = false;
    animatedSpriteComponent->startAnimationTickTime = 0;

    return animatedSpriteComponent;
}

void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, Animation animation) {
    RBE_ASSERT_FMT(animatedSpriteComponent->animationCount + 1 < RBE_MAX_ANIMATIONS,
                   "Adding animation '%s' exceeds the max limit of animations which is '%d'", animation.name, RBE_MAX_ANIMATIONS);
    animatedSpriteComponent->animations[animatedSpriteComponent->animationCount++] = animation;
}

bool animated_sprite_component_set_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name) {
    for (size_t i = 0; i < animatedSpriteComponent->animationCount; i++) {
        if (strcmp(animatedSpriteComponent->animations[i].name, name) == 0) {
            animatedSpriteComponent->currentAnimation = animatedSpriteComponent->animations[i];
            return true;
        }
    }
    return false;
}

AnimationQueryResult animated_sprite_component_get_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name) {
    AnimationQueryResult animationQueryResult;
    for (size_t i = 0; i < animatedSpriteComponent->animationCount; i++) {
        if (strcmp(animatedSpriteComponent->animations[i].name, name) == 0) {
            animationQueryResult.animation = animatedSpriteComponent->animations[i];
            animationQueryResult.success = true;
            return  animationQueryResult;
        }
    }
    animationQueryResult.success = false;
    return animationQueryResult;
}
