#include "parallax_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

ParallaxComponent* parallax_component_create() {
    ParallaxComponent* parallaxComponent = SE_MEM_ALLOCATE(ParallaxComponent);
    parallaxComponent->scrollSpeed = SKA_VECTOR2_ZERO;
    parallaxComponent->cachedScrollSpeed = SKA_VECTOR2_ZERO;
    parallaxComponent->cachedLocalPosition = SKA_VECTOR2_ZERO;
    return parallaxComponent;
}

void parallax_component_delete(ParallaxComponent* parallaxComponent) {
    SE_MEM_FREE(parallaxComponent);
}

ParallaxComponent* parallax_component_copy(const ParallaxComponent* parallaxComponent) {
    ParallaxComponent* copiedComp = SE_MEM_ALLOCATE(ParallaxComponent);
    memcpy(copiedComp, parallaxComponent, sizeof(ParallaxComponent));
    return copiedComp;
}
