#pragma once

#include <seika/math/se_math.h>

typedef struct CreParticle2D {
    SEVector2 position;
    SEVector2 velocity;
    SEVector2 acceleration;
    SEColor color;
    float timeActive;
} CreParticle2D;
