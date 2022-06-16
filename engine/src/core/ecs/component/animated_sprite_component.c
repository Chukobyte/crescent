#include "animated_sprite_component.h"

#include "../../memory/rbe_mem.h"
#include "../../utils/logger.h"

#define RBE_MAX_ANIMATIONS 16

AnimatedSpriteComponent* animated_sprite_component_create() {
    AnimatedSpriteComponent* animatedSpriteComponent = RBE_MEM_ALLOCATE(AnimatedSpriteComponent);
    animatedSpriteComponent->animations = rbe_string_hash_map_create(RBE_MAX_ANIMATIONS);
    animatedSpriteComponent->currentAnimation = NULL;
    animatedSpriteComponent->isPlaying = false;
    animatedSpriteComponent->doesLoop = true;
    animatedSpriteComponent->flipX = false;
    animatedSpriteComponent->flipY = false;

    return animatedSpriteComponent;
}

void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, Animation* animation) {
    if (rbe_string_hash_map_has(animatedSpriteComponent->animations, animation->name)) {
        rbe_logger_warn("Already have animation '%s', not adding!", animation->name);
        return;
    }
    rbe_string_hash_map_add(animatedSpriteComponent->animations, animation->name, animation, sizeof(Animation));
}
