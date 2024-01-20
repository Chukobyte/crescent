#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../physics/particle/particle.h"

#define CRE_PARTICLES_2D_MAX 100

typedef enum Particle2DComponentState {
    Particle2DComponentState_INACTIVE = 0,
    Particle2DComponentState_WAITING_TO_INITIALIZE = 1, // Also used for resets
    Particle2DComponentState_EMITTING = 2
} Particle2DComponentState;

typedef enum Particle2DComponentType {
    Particle2DComponentType_SQUARE = 0,
    Particle2DComponentType_TEXTURE = 1
} Particle2DComponentType;

typedef struct Particle2DTypeTexture {
    struct SETexture* texture;
    SKARect2 drawSource;
} Particle2DTypeTexture;

typedef struct Particles2DComponent {
    // Configuration
    int amount;
    SKAVector2 initialVelocity;
//    SKAVector2 linearAcceleration;
    SKAColor color;
    float spread; // angle in degrees in which the particles are spread
    float lifeTime; // Sets initial 'timeActive' on CreParticle2D
    float damping;
    float explosiveness; // How often particles in an emission cycle are emitted (0.0f - 1.0f), 1.0f meaning the longest delay and all particles come out at once
    Particle2DComponentState state;
    Particle2DComponentType type;
    // Using unions for struct members based on type
    // Shared unions between multiple type
    union {
        SKASize2D squareSize; // Particle2DComponentType_SQUARE | Particle2DComponentType_TEXTURE
    };
    // Specific type structs
    union {
        Particle2DTypeTexture typeTexture;
    };

    CreParticle2D particles[CRE_PARTICLES_2D_MAX];
} Particles2DComponent;

Particles2DComponent* particles2d_component_create();
void particles2d_component_delete(Particles2DComponent* particles2DComponent);
Particles2DComponent* particles2d_component_copy(const Particles2DComponent* particles2DComponent);
// Should be used whenever values changed from the editor // TODO: Refactor particle system to make it robust enough for changes to the component's properties
void particles2d_component_reset_component(Particles2DComponent* particles2DComponent);
void particles2d_component_set_default_particles(Particles2DComponent* particles2DComponent);
void particles2d_component_reset_particle(Particles2DComponent* particles2DComponent, CreParticle2D* particle2D);

#ifdef __cplusplus
}
#endif
