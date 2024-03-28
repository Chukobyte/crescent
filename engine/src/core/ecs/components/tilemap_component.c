#include "tilemap_component.h"

#include <string.h>

#include <seika/memory.h>

#include "../../tilemap/tilemap.h"

TilemapComponent* tilemap_component_create() {
    TilemapComponent* tilemapComponent = SKA_MEM_ALLOCATE(TilemapComponent);
    tilemapComponent->tilemap = cre_tilemap_create_and_initialize();
    return tilemapComponent;
}

void tilemap_component_delete(TilemapComponent* tilemapComponent) {
    SKA_MEM_FREE(tilemapComponent);
}

TilemapComponent* tilemap_component_copy(const TilemapComponent* tilemapComponent) {
    TilemapComponent* copiedComp = SKA_MEM_ALLOCATE(TilemapComponent);
    memcpy(copiedComp, tilemapComponent, sizeof(TilemapComponent));
    return copiedComp;
}
