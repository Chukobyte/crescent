#pragma once

#include <seika/math/se_math.h>

typedef struct ParallaxComponent {
    SKAVector2 scrollSpeed;
    SKAVector2 cachedScrollSpeed; // TODO: Not used, but should be compared and updated once the scripting api can update this value
    SKAVector2 cachedLocalPosition; // TODO: Node can return this from the script side if the real local position is needed there
} ParallaxComponent;

ParallaxComponent* parallax_component_create();
void parallax_component_delete(ParallaxComponent* parallaxComponent);
ParallaxComponent* parallax_component_copy(const ParallaxComponent* parallaxComponent);
