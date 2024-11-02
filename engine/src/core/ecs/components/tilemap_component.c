#include "tilemap_component.h"

#include <string.h>

#include <seika/memory.h>

#include "../../tilemap/tilemap.h"

TilemapComponent* tilemap_component_create() {
    TilemapComponent* tilemapComponent = SKA_ALLOC_ZEROED(TilemapComponent);
    tilemapComponent->tilemap = cre_tilemap_create_and_initialize();
    return tilemapComponent;
}

void tilemap_component_delete(TilemapComponent* tilemapComponent) {
    SKA_FREE(tilemapComponent);
}

TilemapComponent* tilemap_component_copy(const TilemapComponent* tilemapComponent) {
    TilemapComponent* copiedComp = tilemap_component_create();
    memcpy(copiedComp, tilemapComponent, sizeof(TilemapComponent));
    return copiedComp;
}
