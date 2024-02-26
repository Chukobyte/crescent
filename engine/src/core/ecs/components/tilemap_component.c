#include "tilemap_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

#include "../../tilemap/tilemap.h"

TilemapComponent* tilemap_component_create() {
    TilemapComponent* tilemapComponent = SE_MEM_ALLOCATE(TilemapComponent);
    tilemapComponent->tilemap = cre_tilemap_create_and_initialize();
    return tilemapComponent;
}

void tilemap_component_delete(TilemapComponent* tilemapComponent) {
    SE_MEM_FREE(tilemapComponent);
}

TilemapComponent* tilemap_component_copy(const TilemapComponent* tilemapComponent) {
    TilemapComponent* copiedComp = SE_MEM_ALLOCATE(TilemapComponent);
    memcpy(copiedComp, tilemapComponent, sizeof(TilemapComponent));
    return copiedComp;
}
