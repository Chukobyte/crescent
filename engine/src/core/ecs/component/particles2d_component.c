#include "particles2d_component.h"

#include <string.h>

#include <seika/rendering/texture.h>
#include <seika/memory/se_mem.h>

Particles2DComponent* particles2d_component_create() {
    Particles2DComponent* particles2DComponent = SE_MEM_ALLOCATE(Particles2DComponent);
    particles2DComponent->amount = 0;
    particles2DComponent->linearVelocity = (SKAVector2){ 0.0f, 0.0f };
    particles2DComponent->linearAcceleration = (SKAVector2){ 0.0f, 0.0f };
    particles2DComponent->color = (SKAColor){ 1.0f, 1.0f, 1.0f, 1.0f };
    particles2DComponent->lifeTime = 0.0f;
    particles2DComponent->damping = 1.0f;
    particles2DComponent->type = Particle2DComponentType_SQUARE;
    particles2DComponent->squareSize = (SKAVector2){ 1.0f, 1.0f };
    memset(particles2DComponent->particles, 0, CRE_PARTICLES_2D_MAX * sizeof(CreParticle2D));
    return particles2DComponent;
}

void particles2d_component_delete(Particles2DComponent* particles2DComponent) {
    if (particles2DComponent) {
        SE_MEM_FREE(particles2DComponent);
    }
}

Particles2DComponent* particles2d_component_copy(const Particles2DComponent* particles2DComponent) {
    Particles2DComponent* copiedComponent = SE_MEM_ALLOCATE(Particles2DComponent);
    memcpy(copiedComponent, particles2DComponent, sizeof(Particles2DComponent));
    return copiedComponent;
}

void particles2d_component_set_default_particles(Particles2DComponent* particles2DComponent) {
#define DEFAULT_PARTICLE2D (CreParticle2D){ \
    .position = (SKAVector2){ 0.0f, 0.0f }, \
    .velocity = (SKAVector2){ 0.0f, 0.0f }, \
    .acceleration = (SKAVector2){ 0.0f, 0.0f }, \
    .color = (SKAColor){ 1.0f, 1.0f, 1.0f, 1.0f }, \
    .timeActive = 8.0f \
}

    for (int i = 0; i < particles2DComponent->amount; i++) {
        particles2DComponent->particles[i] = DEFAULT_PARTICLE2D;
    }

#undef DEFAULT_PARTICLE2D
}
