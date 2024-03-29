#include "particle.h"

#include <seika/assert.h>

static inline void add_scaled_vector2(SkaVector2* target, const SkaVector2* source, f32 scale) {
    target->x += source->x * scale;
    target->y += source->y * scale;
}

//--- Particle2D ---//

void cre_particle2d_set_mass(CreParticle2D* particle2D, f32 mass) {
    SKA_ASSERT(mass != 0.0f);
    particle2D->inverseMass = 1.0f / mass;
}

f32 cre_particle2d_get_mass(CreParticle2D* particle2D) {
    return particle2D->inverseMass >= 0.0f ? 1.0f / particle2D->inverseMass : FLT_MAX;
}

//--- Particle System ---//

void cre_particle_system_integrate(CreParticle2D* particle2D, f32 duration) {
    if (particle2D->inverseMass <= 0.0f || duration <= 0.0f) {
        return;
    }

    // Update position
    SkaVector2 currentVel = SKA_VECTOR2_ZERO;
    add_scaled_vector2(&currentVel, &particle2D->linearVelocity, duration);
    particle2D->position.x += currentVel.x;
    particle2D->position.y += currentVel.y;

    // Apply forces TODO: Put this somewhere else
    particle2D->forceAccumulated.x += particle2D->gravity.x;
    particle2D->forceAccumulated.y += particle2D->gravity.y;

    // Update acceleration
    SkaVector2 acceleration = particle2D->acceleration;
    add_scaled_vector2(&acceleration, &particle2D->forceAccumulated, particle2D->inverseMass);

    // Update linear velocity
    add_scaled_vector2(&particle2D->linearVelocity, &acceleration, duration);

    // Apply drag
    const f32 dragAmount = powf(particle2D->damping, duration);
    particle2D->linearVelocity.x *= dragAmount;
    particle2D->linearVelocity.y *= dragAmount;

    // Clear out accumulated forces
    particle2D->forceAccumulated = SKA_VECTOR2_ZERO;

    particle2D->timeActive += duration;
}
