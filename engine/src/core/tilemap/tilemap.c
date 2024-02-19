#include "tilemap.h"

#include <seika/rendering/texture.h>
#include <seika/memory/se_mem.h>
#include "seika/utils/se_assert.h"

void cre_tile_array2d_allocate(CreTileArray2D* tileArray) {
    SE_ASSERT(tileArray->array == NULL);
    tileArray->array = (CreTile**)SE_MEM_ALLOCATE_SIZE(tileArray->size.h * sizeof(CreTile*));
    for (int i = 0; i < tileArray->size.h; i++) {
        tileArray->array[i] = (CreTile*) SE_MEM_ALLOCATE_SIZE(tileArray->size.w * sizeof(CreTile));
    }
}

void cre_tile_array2d_deallocate(CreTileArray2D* tileArray) {
    SE_ASSERT(tileArray->array);
    for (int i = 0; i < tileArray->size.h; i++) {
        SE_MEM_FREE(tileArray->array[i]);
    }
    SE_MEM_FREE(tileArray->array);
    tileArray->array = NULL;
}

static inline void cre_tilemap_refresh_tilemap(CreTilemap* tilemap) {
    const int rows = tilemap->tiles.size.h;
    const int cols = tilemap->tiles.size.w;
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            CreTileBitmask bitmask = CreTileType_INVALID;
            if (y - 1 >= 0 && tilemap->tiles.array[y - 1][x].isActive) { bitmask |= CreTileType_TOP; }
            if (x + 1 < rows && tilemap->tiles.array[y][x + 1].isActive) { bitmask |= CreTileType_RIGHT; }
            if (y + 1 < cols && tilemap->tiles.array[y + 1][x].isActive) { bitmask |= CreTileType_BOTTOM; }
            if (x - 1 >= 0 && tilemap->tiles.array[y][x - 1].isActive) { bitmask |= CreTileType_LEFT; }
            if (tilemap->tiles.array[y][x].isActive) { bitmask |= CreTileType_CENTER; }
            tilemap->tiles.array[y][x].bitmask = bitmask;
        }
    }
}

static inline void cre_tilemap_update_active_size(CreTilemap* tilemap, CreTile* updatedTile, const SKAVector2i* position) {
    if (updatedTile->isActive) {
        if (position->x > tilemap->activeSize.w) {
            tilemap->activeSize.w = position->x;
        }
        if (position->y > tilemap->activeSize.h) {
            tilemap->activeSize.h = position->y;
        }
    } else {
        // Only update active size if the current position matches
        if (position->x == tilemap->activeSize.w || position->y == tilemap->activeSize.h) {
            SKASize2Di currentLargestSize = SKA_STRUCT_LITERAL(SKASize2Di){ 0, 0 };
            SE_ASSERT(tilemap->tiles.array);
            for (int x = 0; x < tilemap->activeSize.w; x++) {
                for (int y = 0; y < tilemap->activeSize.h; y++) {
                    if (tilemap->tiles.array[y][x].isActive) {
                        if (x > currentLargestSize.w) {
                            currentLargestSize.w = x;
                        }
                        if (y > currentLargestSize.h) {
                            currentLargestSize.h = y;
                        }
                    }
                }
            }
            tilemap->activeSize = currentLargestSize;
        }
    }
}

void cre_tilemap_set_tile_active(CreTilemap* tilemap, const SKAVector2i* position, bool isActive) {
    SE_ASSERT(tilemap->tiles.array);
    CreTile* tile = &tilemap->tiles.array[position->y][position->x];
    if (tile->isActive != isActive) {
        tile->isActive = isActive;
        cre_tilemap_update_active_size(tilemap, tile, position);
        cre_tilemap_refresh_tilemap(tilemap);
    }
}

bool cre_tilemap_is_tile_active(const CreTilemap* tilemap, const SKAVector2i* position) {
    SE_ASSERT(tilemap->tiles.array);
    return tilemap->tiles.array[position->y][position->x].isActive;
}

CreTileBitmask cre_tilemap_get_tile_bitmask(const CreTilemap* tilemap, const SKAVector2i* position) {
    SE_ASSERT(tilemap->tiles.array);
    return tilemap->tiles.array[position->y][position->x].bitmask;
}
