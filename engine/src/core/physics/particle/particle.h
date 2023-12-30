#pragma once

#include <seika/math/se_math.h>

// Particle

#define CRE_PARTICLE2D_DEFAULT_DAMPING 0.999f

// CPU representation of a particle.
typedef struct CreParticle2D {
    SKATransformModel2D transform;
    SKAVector2 position;
    SKAVector2 linearVelocity;
    SKAVector2 acceleration;
    SKAVector2 forceAccumulated;
    SKAVector2 gravity;
    SKAColor color;
    float timeActive;
    float damping;
    float inverseMass;
} CreParticle2D;

void cre_particle2d_set_mass(CreParticle2D* particle2D, float mass);
float cre_particle2d_get_mass(CreParticle2D* particle2D);

// Particle System

void cre_particle_system_integrate(CreParticle2D* particle2D, float duration);
