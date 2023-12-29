#include "particle.h"

#include <seika/utils/se_assert.h>

static inline void add_scaled_vector2(SKAVector2* target, const SKAVector2* source, float scale) {
    target->x += source->x * scale;
    target->y += source->y * scale;
}

//--- Particle2D ---//

void cre_particle2d_set_mass(CreParticle2D* particle2D, float mass) {
    SE_ASSERT(mass != 0.0f);
    particle2D->inverseMass = 1.0f / mass;
}

float cre_particle2d_get_mass(CreParticle2D* particle2D) {
    return particle2D->inverseMass >= 0.0f ? 1.0f / particle2D->inverseMass : FLT_MAX;
}

//--- Particle System ---//

void cre_particle_system_integrate(CreParticle2D* particle2D, float duration) {
    if (particle2D->inverseMass <= 0.0f) {
        return;
    }

    SE_ASSERT(duration > 0.0f);

    // Update position
    add_scaled_vector2(&particle2D->position, &particle2D->linearVelocity, duration);

    // Apply forces TODO: Put this somewhere else
    particle2D->forceAccumulated.x += particle2D->gravity.x;
    particle2D->forceAccumulated.y += particle2D->gravity.y;

    // Update acceleration
    SKAVector2 acceleration = particle2D->acceleration;
    add_scaled_vector2(&acceleration, &particle2D->forceAccumulated, particle2D->inverseMass);

    // Update linear velocity
    add_scaled_vector2(&particle2D->linearVelocity, &acceleration, duration);

    // Apply drag
    particle2D->linearVelocity.x *= powf(particle2D->damping, duration);
    particle2D->linearVelocity.y *= powf(particle2D->damping, duration);

    // Clear out accumulated forces
    particle2D->forceAccumulated = SKA_VECTOR2_ZERO;

    particle2D->timeActive += duration;
}
