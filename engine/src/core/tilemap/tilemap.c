#include "tilemap.h"

#include <string.h>

#include <seika/memory.h>
#include <seika/assert.h>
#include <seika/rendering/texture.h>

static CreTilemapTransactionItem* get_transaction_item_tail(const CreTilemapTransaction* transaction);
static CreTilemapTransactionItem* find_transaction_item(const CreTilemapTransaction* transaction, const SkaVector2i* position);
static const CreTileData* find_existing_transaction_item_or_tile_data(const CreTilemap* tilemap, const SkaVector2i* position);

void cre_tilemap_initialize(CreTilemap* tilemap) {
    SKA_ASSERT(tilemap->tilesArray == NULL);
    tilemap->tilesArray = ska_array2d_create(0, 0, sizeof(CreTileData));
    tilemap->activeTiles = ska_array_list_create(sizeof(CreTileData*), 128);
}

CreTilemap* cre_tilemap_create_and_initialize() {
    const CreTilemap defaultTilemap = CRE_TILEMAP_DEFAULT_EMPTY;

    CreTilemap* newTilemap = SKA_MEM_ALLOCATE(CreTilemap);
    memcpy(newTilemap, &defaultTilemap, sizeof(CreTilemap));

    cre_tilemap_initialize(newTilemap);

    return newTilemap;
}

void cre_tilemap_finalize(CreTilemap* tilemap) {
    SKA_ASSERT(tilemap->tilesArray);
    ska_array2d_destroy(tilemap->tilesArray);
    ska_array_list_destroy(tilemap->activeTiles);
}

static inline void refresh_tilemap_bitmask(CreTilemap* tilemap) {
    ska_array_list_clear(tilemap->activeTiles);
    const int rows = tilemap->tilesArray->size.w;
    const int cols = tilemap->tilesArray->size.h;
    for (int y = 0; y < cols; y++) {
        for (int x = 0; x < rows; x++) {
            CreTileData* centerTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y);
            // Update active tiles
            if (centerTile->isActive) {
                ska_array_list_push_back(tilemap->activeTiles, &centerTile);
            }
            // Update bitmask
            CreTileBitmask bitmask = CreTileType_INVALID;
            SKA_ASSERT_FMT(centerTile, "Center tile should be present for (%d, %d)", x, y);
            const CreTileData* topTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y - 1);
            const CreTileData* rightTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x + 1, y);
            const CreTileData* bottomTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y + 1);
            const CreTileData* leftTile = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x - 1, y);
            if (centerTile->isActive) { bitmask |= CreTileType_CENTER; }
            if (topTile && y - 1 >= 0 && topTile->isActive) { bitmask |= CreTileType_TOP; }
            if (rightTile && x + 1 < rows && rightTile->isActive) { bitmask |= CreTileType_RIGHT; }
            if (bottomTile && y + 1 < cols && bottomTile->isActive) { bitmask |= CreTileType_BOTTOM; }
            if (x - 1 >= 0 && leftTile->isActive) { bitmask |= CreTileType_LEFT; }
            // Finally write the new bitmask to the tile data
            centerTile->bitmask = bitmask;
        }
    }
}

void cre_tilemap_set_tile_active(CreTilemap* tilemap, const SkaVector2i* position, bool isActive) {
    CreTileData tileData = { .isActive = isActive, .bitmask = 0, .position = *position, .renderCoords = SKA_VECTOR2I_ZERO };
    const CreTileData* existingData = find_existing_transaction_item_or_tile_data(tilemap, position);
    if (existingData) {
        tileData.bitmask = existingData->bitmask;
        tileData.renderCoords = existingData->renderCoords;
    }
    cre_tilemap_set_tile_data(tilemap, &tileData);
}

void cre_tilemap_set_tile_render_coord(CreTilemap* tilemap, const SkaVector2i* position, const SkaVector2i* coord) {
    CreTileData tileData = { .isActive = true, .bitmask = 0, .renderCoords = *coord, .position = *position };
    const CreTileData* existingData = find_existing_transaction_item_or_tile_data(tilemap, position);
    if (existingData) {
        tileData.isActive = existingData->isActive;
        tileData.bitmask = existingData->bitmask;
    }
    cre_tilemap_set_tile_data(tilemap, &tileData);
}

void cre_tilemap_set_tile_data(CreTilemap* tilemap, const CreTileData* tileData) {
    bool shouldValidateTiles = false;
    if (tilemap->activeTransaction == NULL) {
        tilemap->activeTransaction = SKA_MEM_ALLOCATE(CreTilemapTransaction);
        CreTilemapTransactionItem* rootItem = SKA_MEM_ALLOCATE(CreTilemapTransactionItem);
        rootItem->data = *tileData;
        tilemap->activeTransaction->rootItem = rootItem;
        tilemap->activeTransaction->requestedSize = tilemap->tilesArray->size;
        shouldValidateTiles = true;
    } else {
        CreTilemapTransactionItem* item = find_transaction_item(tilemap->activeTransaction, &tileData->position);
        if (!item) {
            item = SKA_MEM_ALLOCATE(CreTilemapTransactionItem);
            // Add new item to tail
            CreTilemapTransactionItem* tailItem = get_transaction_item_tail(tilemap->activeTransaction);
            SKA_ASSERT(tailItem);
            tailItem->next = item;
        }
        // Only validate tiles when one is enabled or disabled
        shouldValidateTiles = item->data.isActive != tileData->isActive;
        item->data = *tileData;
    }

    // Early out if we don't need to validate tiles
    if (!shouldValidateTiles) {
        return;
    }

    // Update requested size
    if (tileData->isActive) {
        if (tileData->position.x + 1 > tilemap->activeTransaction->requestedSize.w) {
            tilemap->activeTransaction->requestedSize.w = tileData->position.x + 1;
        }
        if (tileData->position.y + 1 > tilemap->activeTransaction->requestedSize.h) {
            tilemap->activeTransaction->requestedSize.h = tileData->position.y + 1;
        }
    } else {
        if (tileData->position.x + 1 >= tilemap->activeTransaction->requestedSize.w) {
            int newWidth = 0;
            const int rows = tilemap->tilesArray->size.h;
            const int cols = tilemap->tilesArray->size.w;
            for (int x = rows - 1; x >= 0; x--) {
                for (int y = cols - 1; y >= 0; y--) {
                    if (tileData->position.x != x && tileData->position.y != y) {
                        const CreTileData* data = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y);
                        if (data->isActive) {
                            newWidth = x + 1;
                            break;
                        }
                    }
                }
            }
            tilemap->activeTransaction->requestedSize.w = newWidth;
        }
        if (tileData->position.y + 1 >= tilemap->activeTransaction->requestedSize.h) {
            int newHeight = 0;
            const int rows = tilemap->tilesArray->size.h;
            const int cols = tilemap->tilesArray->size.w;
            for (int x = rows - 1; x >= 0; x--) {
                for (int y = cols - 1; y >= 0; y--) {
                    if (tileData->position.x != x && tileData->position.y != y) {
                        const CreTileData* data = (CreTileData*)ska_array2d_get(tilemap->tilesArray, x, y);
                        if (data->isActive) {
                            newHeight = y + 1;
                            break;
                        }
                    }
                }
            }
            tilemap->activeTransaction->requestedSize.h = newHeight;
        }
    }
}

bool cre_tilemap_is_tile_active(const CreTilemap* tilemap, const SkaVector2i* position) {
    const CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    SKA_ASSERT(tileData);
    return tileData->isActive;
}

void cre_tilemap_clear_all_tiles(const CreTilemap* tilemap) {
    ska_array2d_clear(tilemap->tilesArray);
}

void cre_tilemap_commit_active_tile_changes(CreTilemap* tilemap) {
    if (tilemap->activeTransaction) {
        const SkaSize2Di* requestedSize = &tilemap->activeTransaction->requestedSize;
        // Resize if size changed
        if (requestedSize->w != tilemap->tilesArray->size.w || requestedSize->h != tilemap->tilesArray->size.h) {
            ska_array2d_resize(tilemap->tilesArray, requestedSize->w, requestedSize->h);
        }

        CreTilemapTransactionItem* item = tilemap->activeTransaction->rootItem;
        while (item != NULL) {
            if (item->data.position.x < requestedSize->w && item->data.position.y < requestedSize->h) {
                // Set tile data
                CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, item->data.position.x, item->data.position.y);
                SKA_ASSERT(tileData);
                memcpy(tileData, &item->data, sizeof(CreTileData));
            }
            item = item->next;
        }

        cre_tilemap_clear_active_tile_changes(tilemap);

        refresh_tilemap_bitmask(tilemap);
    }
}

void cre_tilemap_clear_active_tile_changes(CreTilemap* tilemap) {
    if (tilemap->activeTransaction) {
        CreTilemapTransactionItem* item = tilemap->activeTransaction->rootItem;
        while (item != NULL) {
            CreTilemapTransactionItem* itemToDelete = item;
            item = item->next;
            SKA_MEM_FREE(itemToDelete);
        }
        SKA_MEM_FREE(tilemap->activeTransaction);
        tilemap->activeTransaction = NULL;
    }
}

CreTileBitmask cre_tilemap_get_tile_bitmask(const CreTilemap* tilemap, const SkaVector2i* position) {
    const CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    SKA_ASSERT(tileData);
    return tileData->bitmask;
}

const CreTileData* cre_tilemap_get_tile_data(const CreTilemap* tilemap, const SkaVector2i* position) {
    const CreTileData* tileData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    SKA_ASSERT(tileData);
    return tileData;
}

CreTilemapTransactionItem* get_transaction_item_tail(const CreTilemapTransaction* transaction) {
    CreTilemapTransactionItem* item = transaction->rootItem;
    while (item != NULL) {
        if (!item->next) {
            break;
        }
        item = item->next;
    }
    return item;
}

CreTilemapTransactionItem* find_transaction_item(const CreTilemapTransaction* transaction, const SkaVector2i* position) {
    if (transaction) {
        CreTilemapTransactionItem* item = transaction->rootItem;
        while (item != NULL) {
            if (item->data.position.x == position->x && item->data.position.y == position->y) {
                return item;
            }
            item = item->next;
        }
    }
    return NULL;
}

const CreTileData* find_existing_transaction_item_or_tile_data(const CreTilemap* tilemap, const SkaVector2i* position) {
    const CreTilemapTransactionItem* item = find_transaction_item(tilemap->activeTransaction, position);
    if (item) {
        return &item->data;
    }
    // We didn't find the transaction item, so try to get it from tiles array
    const CreTileData* existingData = (CreTileData*)ska_array2d_get(tilemap->tilesArray, position->x, position->y);
    return existingData;
}
