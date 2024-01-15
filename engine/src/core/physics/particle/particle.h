#pragma once

#include <seika/math/se_math.h>

// Particle

#define CRE_PARTICLE2D_DEFAULT_DAMPING 0.999f

typedef enum Particle2DState {
    Particle2DState_INACTIVE = 0,
    Particle2DState_ACTIVE = 1,
    Particle2DState_TIMED_WAITING_TO_BE_ACTIVE = 2
} Particle2DState;

// CPU representation of a particle.
typedef struct CreParticle2D {
    SKAVector2 position;
    SKAVector2 linearVelocity;
    SKAVector2 acceleration;
    SKAVector2 forceAccumulated;
    SKAVector2 gravity;
    SKAColor color;
    float timeActive;
    float damping;
    float inverseMass;
    Particle2DState state;
} CreParticle2D;

void cre_particle2d_set_mass(CreParticle2D* particle2D, float mass);
float cre_particle2d_get_mass(CreParticle2D* particle2D);

// Particle System

void cre_particle_system_integrate(CreParticle2D* particle2D, float duration);
