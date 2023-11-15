#pragma once

#include <stdbool.h>

#include "seika/rendering/texture.h"
#include "seika/math/se_math.h"

#define CRE_MAX_ANIMATION_FRAMES 16

typedef struct CreAnimationFrame {
    SETexture* texture;
    SERect2 drawSource;
    int frame;
} CreAnimationFrame;

typedef struct CreAnimation {
    CreAnimationFrame animationFrames[CRE_MAX_ANIMATION_FRAMES];
    int frameCount;
    int currentFrame;
    int speed;
    char name[16];
    bool doesLoop;
    bool isValid;
} CreAnimation;
