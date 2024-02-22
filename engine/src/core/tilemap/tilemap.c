#include "tilemap.h"

#include <seika/rendering/texture.h>
#include <seika/memory/se_mem.h>
#include "seika/utils/se_assert.h"

void cre_tilemap_initialize(CreTilemap* tilemap) {
    SE_ASSERT(tilemap->tilesArray == NULL);
    tilemap->tilesArray = ska_array2d_create(0, 0, sizeof(CreTileData));
}

void cre_tilemap_finalize(CreTilemap* tilemap) {
    SE_ASSERT(tilemap->tilesArray);
    ska_array2d_destroy(tilemap->tilesArray);
}

static inline void refresh_tilemap_bitmask(CreTilemap* tilemap) {
    const int rows = tilemap->tilesArray->size.h;
    const int cols = tilemap->tilesArray->size.w;
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            CreTileBitmask bitmask = CreTileType_INVALID;
            CreTileData* centerTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y);
            const CreTileData* topTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y - 1);
            const CreTileData* rightTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x + 1, y);
            const CreTileData* bottomTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y + 1);
            const CreTileData* leftTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x - 1, y);
            if (centerTile->isActive) { bitmask |= CreTileType_CENTER; }
            if (y - 1 >= 0 && topTile->isActive) { bitmask |= CreTileType_TOP; }
            if (x + 1 < rows && rightTile->isActive) { bitmask |= CreTileType_RIGHT; }
            if (y + 1 < cols && bottomTile->isActive) { bitmask |= CreTileType_BOTTOM; }
            if (x - 1 >= 0 && leftTile->isActive) { bitmask |= CreTileType_LEFT; }
            // Finally write the new bitmask to the tile data
            centerTile->bitmask = bitmask;
        }
    }
}

void cre_tilemap_set_tile_active(CreTilemap* tilemap, const SKAVector2i* position, bool isActive) {
    if (tilemap->activeTransaction == NULL) {
        tilemap->activeTransaction = SE_MEM_ALLOCATE(CreTilemapTransaction);
        CreTilemapTransactionItem* rootItem = SE_MEM_ALLOCATE(CreTilemapTransactionItem);
        rootItem->position = *position;
        rootItem->isEnabled = isActive;
        tilemap->activeTransaction->rootItem = rootItem;
        tilemap->activeTransaction->requestedSize = tilemap->tilesArray->size;
    } else {
        CreTilemapTransactionItem* item = tilemap->activeTransaction->rootItem;
        while (item != NULL) {
            if (item->next) {
                item = item->next;
            } else {
                break;
            }
        }
        SE_ASSERT(item);
        CreTilemapTransactionItem* newItem = SE_MEM_ALLOCATE(CreTilemapTransactionItem);
        newItem->position = *position;
        newItem->isEnabled = isActive;
        item->next = newItem;
    }

    // Update requested size
    if (isActive) {
        if (position->x + 1 > tilemap->activeTransaction->requestedSize.w) {
            tilemap->activeTransaction->requestedSize.w = position->x + 1;
        }
        if (position->y + 1 > tilemap->activeTransaction->requestedSize.h) {
            tilemap->activeTransaction->requestedSize.h = position->y + 1;
        }
    } else {
        if (position->x + 1 >= tilemap->activeTransaction->requestedSize.w) {
            int newWidth = 0;
            const int rows = tilemap->tilesArray->size.h;
            const int cols = tilemap->tilesArray->size.w;
            for (int x = rows - 1; x >= 0; x--) {
                for (int y = cols - 1; y >= 0; y--) {
                    if (position->x != x && position->y != y) {
                        CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y);
                        if (tileData->isActive) {
                            newWidth = tileData->position.x + 1;
                            break;
                        }
                    }
                }
            }
            tilemap->activeTransaction->requestedSize.w = newWidth;
        }
        if (position->y + 1 >= tilemap->activeTransaction->requestedSize.h) {
            int newHeight = 0;
            const int rows = tilemap->tilesArray->size.h;
            const int cols = tilemap->tilesArray->size.w;
            for (int x = rows - 1; x >= 0; x--) {
                for (int y = cols - 1; y >= 0; y--) {
                    if (position->x != x && position->y != y) {
                        CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y);
                        if (tileData->isActive) {
                            newHeight = tileData->position.y + 1;
                            break;
                        }
                    }
                }
            }
            tilemap->activeTransaction->requestedSize.h = newHeight;
        }
    }
}

bool cre_tilemap_is_tile_active(const CreTilemap* tilemap, const SKAVector2i* position) {
    const CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    SE_ASSERT(tileData);
    return tileData->isActive;
}

static void tilemap_set_tile(CreTilemap* tilemap, const SKAVector2i* position, bool isActive) {
    CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    SE_ASSERT(tileData);
    tileData->isActive = isActive;
}

void cre_tilemap_commit_active_tile_changes(CreTilemap* tilemap) {
    if (tilemap->activeTransaction) {
        const SKASize2Di* requestedSize = &tilemap->activeTransaction->requestedSize;
        // Resize if size changed
        if (requestedSize->w != tilemap->tilesArray->size.w || requestedSize->h != tilemap->tilesArray->size.h) {
            ska_array2d_resize(tilemap->tilesArray, requestedSize->w, requestedSize->h);
        }

        CreTilemapTransactionItem* item = tilemap->activeTransaction->rootItem;
        while (item != NULL) {
            if (item->position.x < requestedSize->w && item->position.y < requestedSize->h) {
                tilemap_set_tile(tilemap, &item->position, item->isEnabled);
            }
            item = item->next;
        }

        SE_MEM_FREE(tilemap->activeTransaction);
        tilemap->activeTransaction = NULL;

        refresh_tilemap_bitmask(tilemap);
    }
}

CreTileBitmask cre_tilemap_get_tile_bitmask(const CreTilemap* tilemap, const SKAVector2i* position) {
    const CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    SE_ASSERT(tileData);
    return tileData->bitmask;
}
