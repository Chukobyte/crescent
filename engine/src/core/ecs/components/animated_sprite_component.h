#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <seika/rendering/shader/shader_instance_minimal.h>
#include <seika/ecs/entity.h>
#include <seika/event.h>
#include <seika/math/math.h>
#include <seika/data_structures/hash_map_string.h>

#include "../../animation/animation.h"

#define ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS 16

typedef struct AnimatedSpriteFrameChangedPayload {
    const SkaEntity entity;
    const int newFrame;
} AnimatedSpriteFrameChangedPayload;

typedef struct AnimatedSpriteAnimationFinishedPayload {
    const SkaEntity entity;
    const CreAnimation* animation;
} AnimatedSpriteAnimationFinishedPayload;

typedef struct AnimatedSpriteComponent {
    CreAnimation animations[ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS];
    CreAnimation currentAnimation;
    size_t animationCount;
    SkaColor modulate;
    bool isPlaying;
    SkaVector2 origin;
    bool flipH;
    bool flipV;
    uint32_t startAnimationTickTime;
    bool staggerStartAnimationTimes; // If true, will apply a random start time modifier when being played
    uint32_t randomStaggerTime; // Used to stagger animations
    SkaShaderInstanceId shaderInstanceId;
    SkaEvent onFrameChanged; // { data = AnimatedSpriteFrameChangedPayload(self) }
    SkaEvent onAnimationFinished; // { data = AnimatedSpriteAnimationFinishedPayload(self) }
} AnimatedSpriteComponent;

typedef struct AnimationQueryResult {
    CreAnimation animation;
    bool success;
} AnimationQueryResult;

typedef struct AnimationFrameData {
    char texturePath[64];
    SkaRect2 drawSource;
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
    SkaColor modulate;
    bool isPlaying;
    SkaVector2 origin;
    bool flipH;
    bool flipV;
    bool staggerStartAnimationTimes;
} AnimatedSpriteComponentData;

AnimatedSpriteComponent* animated_sprite_component_create();
void animated_sprite_component_delete(AnimatedSpriteComponent* animatedSpriteComponent);
AnimatedSpriteComponent* animated_sprite_component_copy(const AnimatedSpriteComponent* animatedSpriteComponent);
// TODO: Validate if animation with name already exist
void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, CreAnimation animation);
AnimationQueryResult animated_sprite_component_get_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);
bool animated_sprite_component_set_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);
// Plays animation and returns true if a new one is played
bool animated_sprite_component_play_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);
void animated_sprite_component_refresh_random_stagger_animation_time(AnimatedSpriteComponent* animatedSpriteComponent);

AnimatedSpriteComponentData* animated_sprite_component_data_create();
void animated_sprite_component_data_delete(AnimatedSpriteComponentData* animatedSpriteComponent);
AnimatedSpriteComponent* animated_sprite_component_data_copy_to_animated_sprite(const AnimatedSpriteComponentData* animatedSpriteComponent);
void animated_sprite_component_data_add_animation(AnimatedSpriteComponentData* animatedSpriteComponent, AnimationData animation);

#ifdef __cplusplus
}
#endif
