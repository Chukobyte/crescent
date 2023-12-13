#include "animated_sprite_component.h"

#include <string.h>

#include <seika/seika.h>
#include <seika/asset/asset_manager.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#define RBE_MAX_ANIMATIONS 16

AnimatedSpriteComponent* animated_sprite_component_create() {
    AnimatedSpriteComponent* animatedSpriteComponent = SE_MEM_ALLOCATE(AnimatedSpriteComponent);
    animatedSpriteComponent->animationCount = 0;
    animatedSpriteComponent->modulate.r = 1.0f;
    animatedSpriteComponent->modulate.g = 1.0f;
    animatedSpriteComponent->modulate.b = 1.0f;
    animatedSpriteComponent->modulate.a = 1.0f;
    animatedSpriteComponent->currentAnimation.isValid = false;
    animatedSpriteComponent->origin.x = 0.0f;
    animatedSpriteComponent->origin.y = 0.0f;
    animatedSpriteComponent->isPlaying = false;
    animatedSpriteComponent->flipH = false;
    animatedSpriteComponent->flipV = false;
    animatedSpriteComponent->startAnimationTickTime = 0;
    animatedSpriteComponent->staggerStartAnimationTimes = false;
    animatedSpriteComponent->randomStaggerTime = 0;
    animatedSpriteComponent->shaderInstanceId = SE_SHADER_INSTANCE_INVALID_ID;
    animatedSpriteComponent->onFrameChanged.observerCount = 0;
    animatedSpriteComponent->onAnimationFinished.observerCount = 0;

    return animatedSpriteComponent;
}

void animated_sprite_component_delete(AnimatedSpriteComponent* animatedSpriteComponent) {
    SE_MEM_FREE(animatedSpriteComponent);
}

AnimatedSpriteComponent* animated_sprite_component_copy(const AnimatedSpriteComponent* animatedSpriteComponent) {
    AnimatedSpriteComponent* copiedNode = SE_MEM_ALLOCATE(AnimatedSpriteComponent);
    memcpy(copiedNode, animatedSpriteComponent, sizeof(AnimatedSpriteComponent));
    return copiedNode;
}

void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, CreAnimation animation) {
    SE_ASSERT_FMT(animatedSpriteComponent->animationCount + 1 < RBE_MAX_ANIMATIONS,
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

bool animated_sprite_component_play_animation(AnimatedSpriteComponent *animatedSpriteComponent, const char* name) {
    // First checks if new animation, if not just play the currently set animation (if not playing)
    bool isPlayingNewAnimation = false;
    bool playAnimationSuccess = false;
    if (strcmp(animatedSpriteComponent->currentAnimation.name, name) != 0) {
        playAnimationSuccess = animated_sprite_component_set_animation(animatedSpriteComponent, name);
        if (playAnimationSuccess) {
            isPlayingNewAnimation = true;
        }
    } else if (!animatedSpriteComponent->isPlaying) {
        const AnimationQueryResult queryResult = animated_sprite_component_get_animation(animatedSpriteComponent, name);
        if (queryResult.success) {
            playAnimationSuccess = true;
        }
    }
    if (playAnimationSuccess) {
        animatedSpriteComponent->isPlaying = true;
        animatedSpriteComponent->startAnimationTickTime = sf_get_ticks() + animatedSpriteComponent->randomStaggerTime;
    }
    return isPlayingNewAnimation;
}

void animated_sprite_component_refresh_random_stagger_animation_time(AnimatedSpriteComponent* animatedSpriteComponent) {
    if (animatedSpriteComponent->staggerStartAnimationTimes) {
        uint32_t random_value = (rand() & 0xFFFF) << 16; // Generate random upper 16 bits
        random_value |= (rand() & 0xFFFF);
        animatedSpriteComponent->randomStaggerTime = random_value;
    } else {
        animatedSpriteComponent->randomStaggerTime = 0;
    }
}

//--- Animated Sprite Component Data ---//
AnimatedSpriteComponentData* animated_sprite_component_data_create() {
    AnimatedSpriteComponentData* animatedSpriteComponent = SE_MEM_ALLOCATE(AnimatedSpriteComponentData);
    animatedSpriteComponent->animationCount = 0;
    animatedSpriteComponent->modulate.r = 1.0f;
    animatedSpriteComponent->modulate.g = 1.0f;
    animatedSpriteComponent->modulate.b = 1.0f;
    animatedSpriteComponent->modulate.a = 1.0f;
    animatedSpriteComponent->currentAnimation.isValid = false;
    animatedSpriteComponent->origin.x = 0.0f;
    animatedSpriteComponent->origin.y = 0.0f;
    animatedSpriteComponent->isPlaying = false;
    animatedSpriteComponent->flipH = false;
    animatedSpriteComponent->flipV = false;
    animatedSpriteComponent->staggerStartAnimationTimes = false;

    return animatedSpriteComponent;
}

void animated_sprite_component_data_delete(AnimatedSpriteComponentData* animatedSpriteComponent) {
    SE_MEM_FREE(animatedSpriteComponent);
}

AnimatedSpriteComponent* animated_sprite_component_data_copy_to_animated_sprite(const AnimatedSpriteComponentData* animatedSpriteComponentData) {
    AnimatedSpriteComponent* copiedNode = SE_MEM_ALLOCATE(AnimatedSpriteComponent);
    copiedNode->animationCount = animatedSpriteComponentData->animationCount;
    copiedNode->modulate = animatedSpriteComponentData->modulate;
    copiedNode->origin = animatedSpriteComponentData->origin;
    copiedNode->isPlaying = animatedSpriteComponentData->isPlaying;
    copiedNode->flipH = animatedSpriteComponentData->flipH;
    copiedNode->flipV = animatedSpriteComponentData->flipV;
    copiedNode->staggerStartAnimationTimes = animatedSpriteComponentData->staggerStartAnimationTimes;
    se_strcpy(copiedNode->currentAnimation.name, animatedSpriteComponentData->currentAnimation.name);
    for (size_t animationIndex = 0; animationIndex < animatedSpriteComponentData->animationCount; animationIndex++) {
        const AnimationData* animationData = &animatedSpriteComponentData->animations[animationIndex];
        CreAnimation animation;
        se_strcpy(animation.name, animationData->name);
        animation.doesLoop = animationData->doesLoop;
        animation.speed = animationData->speed;
        animation.isValid = true;
        animation.currentFrame = 0;
        animation.frameCount = animationData->frameCount;
        for (size_t frameIndex = 0; (int) frameIndex < animationData->frameCount; frameIndex++) {
            const AnimationFrameData* animationFrameData = &animationData->animationFrames[frameIndex];
            CreAnimationFrame animationFrame;
            animationFrame.texture = se_asset_manager_get_texture(animationFrameData->texturePath);
            animationFrame.drawSource = animationFrameData->drawSource;
            animationFrame.frame = animationFrameData->frame;
            animation.animationFrames[frameIndex] = animationFrame;
        }
        copiedNode->animations[animationIndex] = animation;
        if (strcmp(animatedSpriteComponentData->currentAnimation.name, animation.name) == 0) {
            copiedNode->currentAnimation = animation;
        }
    }

    return copiedNode;
}

void animated_sprite_component_data_add_animation(AnimatedSpriteComponentData* animatedSpriteComponent, AnimationData animation) {
    SE_ASSERT_FMT(animatedSpriteComponent->animationCount + 1 < RBE_MAX_ANIMATIONS,
                  "Adding animation '%s' exceeds the max limit of animations which is '%d'", animation.name, RBE_MAX_ANIMATIONS);
    animatedSpriteComponent->animations[animatedSpriteComponent->animationCount++] = animation;
}
