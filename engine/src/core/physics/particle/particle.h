#pragma once

#include <seika/math/se_math.h>

// Particle

// CPU representation of a particle.
typedef struct CreParticle2D {
    SKAVector2 position;
    SKAVector2 linearVelocity;
    SKAVector2 acceleration;
    SKAColor color;
    float timeActive;
    float damping;
    float inverseMass;
} CreParticle2D;

void cre_particle2d_set_mass(CreParticle2D* particle2D, float mass);
float cre_particle2d_get_mass(CreParticle2D* particle2D);

// Particle System

void cre_particle_system_integrate(CreParticle2D* particle2D, float duration);
