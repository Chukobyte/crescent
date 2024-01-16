#include "particles2d_component.h"

#include <string.h>

#include <seika/rendering/texture.h>
#include <seika/memory/se_mem.h>

static inline float math_vec2_length(const SKAVector2* vector) {
    return sqrtf(vector->x * vector->x + vector->y * vector->y);
}

static inline void math_vec2_normalize(SKAVector2* vector) {
    const float length = math_vec2_length(vector);
    if (length != 0.0f) {
        vector->x /= length;
        vector->y /= length;
    }
}

static inline float get_random_sign() {
    return ((rand() % 2) == 0) ? 1.0f : -1.0f;
}

static inline float get_random_spread_angle_in_radians(SKAVector2* direction, float spreadDegrees) {
    math_vec2_normalize(direction);
    const float dirAngle = ska_math_vec2_angle(direction);
    // Generate a random angle based on direction and spread
    const float randomAngle = SKA_DEG_2_RADF(fmodf((float)rand(), spreadDegrees / 2.0f)) * get_random_sign();
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
    particles2DComponent->spread = 45.0f;
    particles2DComponent->color = SKA_COLOR_WHITE;
    particles2DComponent->lifeTime = 4.0f;
    particles2DComponent->damping = 1.0f;
    particles2DComponent->explosiveness = 0.0f;
    particles2DComponent->state = Particle2DComponentState_WAITING_TO_INITIALIZE; // Defaulting to on for now
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
    .color = particles2DComponent->color, \
    .timeActive = 0.0f, \
    .damping = CRE_PARTICLE2D_DEFAULT_DAMPING, \
    .inverseMass = 1.0f, \
    .state = Particle2DState_INACTIVE\
}

    for (int i = 0; i < particles2DComponent->amount; i++) {
        particles2DComponent->particles[i] = DEFAULT_PARTICLE2D;
        particles2d_component_reset_particle(particles2DComponent, &particles2DComponent->particles[i]);
    }

#undef DEFAULT_PARTICLE2D
}

void particles2d_component_reset_particle(Particles2DComponent* particles2DComponent, CreParticle2D* particle2D) {
    // Reset relevant properties
    particle2D->position = SKA_VECTOR2_ZERO;
    particle2D->linearVelocity = SKA_VECTOR2_ZERO;
    particle2D->acceleration = SKA_VECTOR2_ZERO;
    particle2D->forceAccumulated = SKA_VECTOR2_ZERO;
    particle2D->timeActive = 0.0f;
    // Add initial velocity and spread
    const bool hasInitialVelocity = particles2DComponent->initialVelocity.x != 0.0f || particles2DComponent->initialVelocity.y != 0.0f;
    if (hasInitialVelocity) {
        SKAVector2 initialVelocity = particles2DComponent->initialVelocity;
        const float spreadAngle = get_random_spread_angle_in_radians(&initialVelocity, particles2DComponent->spread);
        particle2D->linearVelocity.x += cosf(spreadAngle) * particles2DComponent->initialVelocity.x;
        particle2D->linearVelocity.y -= sinf(spreadAngle) * particles2DComponent->initialVelocity.y;
    }
}
