#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TilemapComponent {
    struct CreTilemap* tilemap;
} TilemapComponent;

TilemapComponent* tilemap_component_create();
void tilemap_component_delete(TilemapComponent* tilemapComponent);
TilemapComponent* tilemap_component_copy(const TilemapComponent* tilemapComponent);

#ifdef __cplusplus
}
#endif