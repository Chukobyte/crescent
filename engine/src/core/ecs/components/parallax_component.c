#include "parallax_component.h"

#include <string.h>

#include <seika/memory.h>

ParallaxComponent* parallax_component_create() {
    ParallaxComponent* parallaxComponent = SKA_MEM_ALLOCATE(ParallaxComponent);
    return parallaxComponent;
}

void parallax_component_delete(ParallaxComponent* parallaxComponent) {
    SKA_MEM_FREE(parallaxComponent);
}

ParallaxComponent* parallax_component_copy(const ParallaxComponent* parallaxComponent) {
    ParallaxComponent* copiedComp = SKA_MEM_ALLOCATE(ParallaxComponent);
    memcpy(copiedComp, parallaxComponent, sizeof(ParallaxComponent));
    return copiedComp;
}
