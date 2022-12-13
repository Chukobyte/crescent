#pragma once

#include "../seika/src/math/se_math.h"

typedef struct ParallaxComponent {
    Vector2 scrollSpeed;
    Vector2 cachedScrollSpeed;
    Vector2 cachedLocalPosition; // TODO: Node can return this from the script side if the real local position is needed there
} ParallaxComponent;

ParallaxComponent* parallax_component_create();
void parallax_component_delete(ParallaxComponent* parallaxComponent);
ParallaxComponent* parallax_component_copy(const ParallaxComponent* parallaxComponent);
