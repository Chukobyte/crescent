#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/math/se_math.h>

typedef struct Particles2DComponent {
    SEVector2 position;
    SEVector2 linearVelocity;
    SEVector2 linearAcceleration;
    SEColor color;
    float lifeTime;
    float damping;
} Particles2DComponent;

Particles2DComponent* particles2d_component_create();
void particles2d_component_delete(Particles2DComponent* collider2DComponent);
Particles2DComponent* particles2d_component_copy(const Particles2DComponent* collider2DComponent);

#ifdef __cplusplus
}
#endif