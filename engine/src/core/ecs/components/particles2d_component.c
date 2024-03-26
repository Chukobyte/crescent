#include "particles2d_component.h"

#include <string.h>

#include <seika/rendering/texture.h>
#include <seika/memory.h>

static inline f32 math_vec2_length(const SkaVector2* vector) {
    return sqrtf(vector->x * vector->x + vector->y * vector->y);
}

static inline void math_vec2_normalize(SkaVector2* vector) {
    const f32 length = math_vec2_length(vector);
    if (length != 0.0f) {
        vector->x /= length;
        vector->y /= length;
    }
}

static inline f32 get_random_sign() {
    return ((rand() % 2) == 0) ? 1.0f : -1.0f;
}

static inline f32 get_random_spread_angle_in_radians(const SkaVector2* direction, f32 spreadDegrees) {
    const f32 dirAngle = ska_math_vec2_angle(direction);
    // Generate a random angle based on direction and spread
//    const float randomAngle = SKA_DEG_2_RADF(fmodf((float)rand(), spreadDegrees / 2.0f)) * get_random_sign();
    const float randomAngle = SKA_DEG_2_RADF(fmodf((float)rand(), spreadDegrees) - spreadDegrees / 2.0f) * get_random_sign();
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
    Particles2DComponent* particles2DComponent = SKA_MEM_ALLOCATE(Particles2DComponent);
    particles2DComponent->amount = 8;
    particles2DComponent->initialVelocity = SKA_MINMAX_VEC2_ZERO;
    particles2DComponent->spread = 45.0f;
    particles2DComponent->color = SKA_COLOR_WHITE;
    particles2DComponent->lifeTime = 4.0f;
    particles2DComponent->damping = 1.0f;
    particles2DComponent->explosiveness = 0.0f;
    particles2DComponent->state = Particle2DComponentState_WAITING_TO_INITIALIZE; // Defaulting to on for now
    particles2DComponent->type = Particle2DComponentType_SQUARE;
    particles2DComponent->squareSize = (SkaSize2D){ .w = 4.0f, .h = 4.0f };
    memset(particles2DComponent->particles, 0, CRE_PARTICLES_2D_MAX * sizeof(CreParticle2D));
    return particles2DComponent;
}

void particles2d_component_delete(Particles2DComponent* particles2DComponent) {
    if (particles2DComponent) {
        SKA_MEM_FREE(particles2DComponent);
    }
}

Particles2DComponent* particles2d_component_copy(const Particles2DComponent* particles2DComponent) {
    Particles2DComponent* copiedComponent = SKA_MEM_ALLOCATE(Particles2DComponent);
    memcpy(copiedComponent, particles2DComponent, sizeof(Particles2DComponent));
    return copiedComponent;
}

void particles2d_component_reset_component(Particles2DComponent* particles2DComponent) {
    particles2DComponent->state = Particle2DComponentState_WAITING_TO_INITIALIZE;
}

void particles2d_component_set_default_particles(Particles2DComponent* particles2DComponent) {
    for (int32 i = 0; i < particles2DComponent->amount; i++) {
        particles2DComponent->particles[i] = CRE_PARTICLE2D_DEFAULT;
        particles2d_component_reset_particle(particles2DComponent, &particles2DComponent->particles[i]);
    }
}

void particles2d_component_reset_particle(Particles2DComponent* particles2DComponent, CreParticle2D* particle2D) {
    // Reset relevant properties
    particle2D->position = SKA_VECTOR2_ZERO;
    particle2D->linearVelocity = SKA_VECTOR2_ZERO;
    particle2D->acceleration = SKA_VECTOR2_ZERO;
    particle2D->forceAccumulated = SKA_VECTOR2_ZERO;
    particle2D->timeActive = 0.0f;
    particle2D->color = particles2DComponent->color;
    particle2D->prevTransform = SKA_TRANSFORM_IDENTITY;
    // Add initial velocity and spread
    const bool hasInitialVelocity = particles2DComponent->initialVelocity.min.x != 0.0f || particles2DComponent->initialVelocity.min.y != 0.0f;
    if (hasInitialVelocity) {
        const SkaVector2 initialVelocity = ska_math_minmax_vec2_get_random_in_range(&particles2DComponent->initialVelocity);
        particle2D->linearVelocity.x += initialVelocity.x;
        particle2D->linearVelocity.y += initialVelocity.y;

        // Get random angle based on the degrees of spread
        SkaVector2 initialDirection = initialVelocity;
        math_vec2_normalize(&initialDirection);
        const float spreadAngle = get_random_spread_angle_in_radians(&initialDirection, particles2DComponent->spread);
//        const float initialDirectionAngleDegrees = ska_math_vec2_angle(&initialDirection);
//        const float spreadAngleDegrees = SKA_RAD_2_DEGF(spreadAngle);

        const SkaVector2 rotatedVelocity = (SkaVector2){
            .x = initialVelocity.x * cosf(spreadAngle) - initialVelocity.y * sinf(spreadAngle),
            .y = initialVelocity.x * sinf(spreadAngle) - initialVelocity.y * cosf(spreadAngle)
        };

        particle2D->linearVelocity.x += rotatedVelocity.x;
        particle2D->linearVelocity.y += rotatedVelocity.y;


//        particle2D->linearVelocity.x += cosf(spreadAngle) * particles2DComponent->initialVelocity.x;
//        particle2D->linearVelocity.y -= sinf(spreadAngle) * particles2DComponent->initialVelocity.y;
    }
}
