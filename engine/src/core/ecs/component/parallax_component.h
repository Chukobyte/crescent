#pragma once

#include "../seika/src/math/se_math.h"

typedef struct ParallaxComponent {
    Vector2 scrollSpeed;
    Vector2 offset;
} ParallaxComponent;

ParallaxComponent* parallax_component_create();
void parallax_component_delete(ParallaxComponent* parallaxComponent);
ParallaxComponent* parallax_component_copy(const ParallaxComponent* parallaxComponent);
