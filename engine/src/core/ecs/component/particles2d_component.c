#include "particles2d_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

Particles2DComponent* particles2d_component_create() {
    Particles2DComponent* particles2DComponent = SE_MEM_ALLOCATE(Particles2DComponent);
    particles2DComponent->position = (SEVector2){ 0.0f, 0.0f };
    particles2DComponent->linearVelocity = (SEVector2){ 0.0f, 0.0f };
    particles2DComponent->linearAcceleration = (SEVector2){ 0.0f, 0.0f };
    particles2DComponent->color = (SEColor){ 1.0f, 1.0f, 1.0f, 1.0f };
    particles2DComponent->lifeTime = 0.0f;
    particles2DComponent->damping = 1.0f;
    return particles2DComponent;
}

void particles2d_component_delete(Particles2DComponent* collider2DComponent) {
    if (collider2DComponent) {
        SE_MEM_FREE(collider2DComponent);
    }
}

Particles2DComponent* particles2d_component_copy(const Particles2DComponent* particles2DComponent) {
    Particles2DComponent* copiedComponent = SE_MEM_ALLOCATE(Particles2DComponent);
    memcpy(copiedComponent, particles2DComponent, sizeof(Particles2DComponent));
    return copiedComponent;
}
