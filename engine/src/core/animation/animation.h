#pragma once

#include <stdbool.h>

#include "../rendering/texture.h"
#include "../math/rbe_math.h"

#define RBE_MAX_ANIMATION_FRAMES 8

typedef struct AnimationFrame {
    Texture* texture;
    Rect2 drawSource;
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
