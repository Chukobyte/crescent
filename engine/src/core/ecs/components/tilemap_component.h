#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/math/math.h>

struct SkaTexture;
struct CreTilemap;

typedef struct TilemapComponent {
    struct CreTilemap* tilemap;
    SkaVector2 origin; // Used to calculate the global starting position of the first tile (0, 0)
} TilemapComponent;

TilemapComponent* tilemap_component_create();
void tilemap_component_delete(TilemapComponent* tilemapComponent);
TilemapComponent* tilemap_component_copy(const TilemapComponent* tilemapComponent);

#ifdef __cplusplus
}
#endif
