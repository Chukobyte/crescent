#pragma once

#include <seika/math/se_math.h>

typedef struct CreParticle2D {
    SKAVector2 position;
    SKAVector2 velocity;
    SKAVector2 acceleration;
    SKAColor color;
    float timeActive;
} CreParticle2D;
