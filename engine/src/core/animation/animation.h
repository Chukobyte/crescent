#pragma once

#include <stdbool.h>

#include <seika/rendering/texture.h>
#include <seika/math/math.h>

#define CRE_MAX_ANIMATION_FRAMES 16

typedef struct CreAnimationFrame {
    SkaTexture* texture;
    SkaRect2 drawSource;
    int32 frame;
} CreAnimationFrame;

typedef struct CreAnimation {
    CreAnimationFrame animationFrames[CRE_MAX_ANIMATION_FRAMES];
    int32 frameCount;
    int32 currentFrame;
    int32 speed;
    char name[16];
    bool doesLoop;
    bool isValid;
} CreAnimation;
