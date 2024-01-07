#include "particles2d_component.h"

#include <string.h>

#include <seika/rendering/texture.h>
#include <seika/memory/se_mem.h>

static inline float get_random_spread_angle_in_radians(SKAVector2* direction, float spreadDegrees) {
    const float dirAngle = ska_math_vec2_angle(direction);
    const float spreadRadians = SKA_DEG_2_RADF(spreadDegrees);
    // Generate a random angle based on direction and spread
    const float randomAngle = SKA_DEG_2_RADF(fmod(rand(), spreadDegrees));
    const float finalAngle = dirAngle + randomAngle;
    // Ensure the result is within [0, 2π]
    if (finalAngle < 0.0f) {
        return finalAngle + 2.0f * SKA_PI;
    } else if (finalAngle >= 2.0f * SKA_PI) {
        return finalAngle - 2.0f * SKA_PI;
    }
    return finalAngle;
}

Particles2DComponent* particles2d_component_create() {
    Particles2DComponent* particles2DComponent = SE_MEM_ALLOCATE(Particles2DComponent);
    particles2DComponent->amount = 8;
    particles2DComponent->initialVelocity = SKA_VECTOR2_ZERO;
    particles2DComponent->linearAcceleration = SKA_VECTOR2_ZERO;
    particles2DComponent->direction = SKA_VECTOR2_RIGHT;
    particles2DComponent->spread = 45.0f;
    particles2DComponent->color = SKA_COLOR_WHITE;
    particles2DComponent->lifeTime = 0.0f;
    particles2DComponent->damping = 1.0f;
    particles2DComponent->type = Particle2DComponentType_SQUARE;
    particles2DComponent->squareSize = SKA_VECTOR2_ONE;
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
    .position = SKA_VECTOR2_ZERO, \
    .linearVelocity = SKA_VECTOR2_ZERO, \
    .acceleration = SKA_VECTOR2_ZERO, \
    .forceAccumulated = SKA_VECTOR2_ZERO, \
    .gravity = (SKAVector2){ 0.0f, 32.0f }, \
    .angle = 0.0f, \
    .color = particles2DComponent->color, \
    .timeActive = 0.0f, \
    .damping = CRE_PARTICLE2D_DEFAULT_DAMPING, \
    .inverseMass = 1.0f \
}

    const bool hasInitialVelocity = particles2DComponent->initialVelocity.x != 0.0f || particles2DComponent->initialVelocity.y != 0.0f;
    for (int i = 0; i < particles2DComponent->amount; i++) {
        particles2DComponent->particles[i] = DEFAULT_PARTICLE2D;
        CreParticle2D* currentParticle = &particles2DComponent->particles[i];
        // Initial velocity
        if (hasInitialVelocity) {
            currentParticle->linearVelocity.x += particles2DComponent->initialVelocity.x;
            currentParticle->linearVelocity.y += particles2DComponent->initialVelocity.y;
        }
        // Spread angle converting to radians
        currentParticle->angle = get_random_spread_angle_in_radians(&particles2DComponent->direction, particles2DComponent->spread);
    }

#undef DEFAULT_PARTICLE2D
}
