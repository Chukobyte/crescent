#pragma once

#include <stdbool.h>

#include "../seika/src/rendering/texture.h"
#include "../seika/src/math/se_math.h"

#define RBE_MAX_ANIMATION_FRAMES 16

typedef struct AnimationFrame {
    SETexture* texture;
    SERect2 drawSource;
    int frame;
} AnimationFrame;

typedef struct Animation {
    AnimationFrame animationFrames[RBE_MAX_ANIMATION_FRAMES];
    int frameCount;
    int currentFrame;
    int speed;
    char name[16];
    bool doesLoop;
    bool isValid;
} Animation;
