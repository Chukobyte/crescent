#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../physics/particle/particle.h"

#define CRE_PARTICLES_2D_MAX 100

typedef enum Particle2DComponentType {
    Particle2DComponentType_SQUARE = 0,
    Particle2DComponentType_TEXTURE = 1
} Particle2DComponentType;

typedef struct Particles2DComponent {
    // Configuration
    int amount;
    SKAVector2 initialVelocity;
    SKAVector2 linearAcceleration;
    SKAColor color;
    // Sets initial 'timeActive' on CreParticle2D
    float lifeTime;
    float damping;
    Particle2DComponentType type;
    // Using unions for struct members based on type
    union {
        struct SETexture* texture; // Particle2DComponentType_SQUARE
        SKAVector2 squareSize; // Particle2DComponentType_TEXTURE
    };

    CreParticle2D particles[CRE_PARTICLES_2D_MAX];
} Particles2DComponent;

Particles2DComponent* particles2d_component_create();
void particles2d_component_delete(Particles2DComponent* particles2DComponent);
Particles2DComponent* particles2d_component_copy(const Particles2DComponent* particles2DComponent);
void particles2d_component_set_default_particles(Particles2DComponent* particles2DComponent);

#ifdef __cplusplus
}
#endif
