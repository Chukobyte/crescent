#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/math/se_math.h>

#define CRE_PARTICLES_2D_MAX 100

typedef struct CreParticle2D {
    SEVector2 position;
    SEVector2 velocity;
    SEVector2 acceleration;
    SEColor color;
    float timeActive;
} CreParticle2D;

typedef struct Particles2DComponent {
    // Configuration
    SEVector2 linearVelocity;
    SEVector2 linearAcceleration;
    SEColor color;
    float lifeTime;
    float damping;
    struct SETexture* texture;
    CreParticle2D particles[CRE_PARTICLES_2D_MAX];
} Particles2DComponent;

Particles2DComponent* particles2d_component_create();
void particles2d_component_delete(Particles2DComponent* collider2DComponent);
Particles2DComponent* particles2d_component_copy(const Particles2DComponent* collider2DComponent);

#ifdef __cplusplus
}
#endif