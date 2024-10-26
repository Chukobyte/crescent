#include "parallax_component.h"

#include <string.h>

#include <seika/memory.h>

ParallaxComponent* parallax_component_create() {
    return SKA_ALLOC_ZEROED(ParallaxComponent);
}

void parallax_component_delete(ParallaxComponent* parallaxComponent) {
    SKA_FREE(parallaxComponent);
}

ParallaxComponent* parallax_component_copy(const ParallaxComponent* parallaxComponent) {
    ParallaxComponent* copiedComp = SKA_ALLOC(ParallaxComponent);
    memcpy(copiedComp, parallaxComponent, sizeof(ParallaxComponent));
    return copiedComp;
}
