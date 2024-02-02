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
    SKATransform2D prevTransform;
} CreParticle2D;

#define CRE_PARTICLE2D_DEFAULT SKA_STRUCT_LITERAL(CreParticle2D){ \
    .position = SKA_VECTOR2_ZERO, \
    .linearVelocity = SKA_VECTOR2_ZERO, \
    .acceleration = SKA_VECTOR2_ZERO, \
    .forceAccumulated = SKA_VECTOR2_ZERO, \
    .gravity = (SKAVector2){ 0.0f, 32.0f }, \
    .color = particles2DComponent->color, \
    .timeActive = 0.0f, \
    .damping = CRE_PARTICLE2D_DEFAULT_DAMPING, \
    .inverseMass = 1.0f, \
    .state = Particle2DState_INACTIVE, \
    .prevTransform = SKA_TRANSFORM_IDENTITY \
}

void cre_particle2d_set_mass(CreParticle2D* particle2D, float mass);
float cre_particle2d_get_mass(CreParticle2D* particle2D);

// Particle System

void cre_particle_system_integrate(CreParticle2D* particle2D, float duration);
