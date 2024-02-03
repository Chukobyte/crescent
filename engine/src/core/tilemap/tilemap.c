#include "tilemap.h"

static inline void cre_tilemap_refresh_tilemap(CreTilemap* tilemap) {
    for (int x = 0; x < CRE_TILEMAP_MAX_ROWS; x++) {
        for (int y = 0; y < CRE_TILEMAP_MAX_COLS; y++) {
            CreTileBitmask bitmask = CreTileType_INVALID;
            if (y - 1 >= 0 && tilemap->tiles[y - 1][x].isActive) { bitmask |= CreTileType_TOP; }
            if (x + 1 < CRE_TILEMAP_MAX_ROWS && tilemap->tiles[y][x + 1].isActive) { bitmask |= CreTileType_RIGHT; }
            if (y + 1 < CRE_TILEMAP_MAX_COLS && tilemap->tiles[y + 1][x].isActive) { bitmask |= CreTileType_BOTTOM; }
            if (x - 1 >= 0 && tilemap->tiles[y][x - 1].isActive) { bitmask |= CreTileType_LEFT; }
            if (tilemap->tiles[y][x].isActive) { bitmask |= CreTileType_CENTER; }
            tilemap->tiles[y][x].bitmask = bitmask;
        }
    }
}

static inline void cre_tilemap_update_active_size(CreTilemap* tilemap, CreTile* updatedTile, SKAVector2i* position) {
    if (updatedTile->isActive) {
        if (position->x > tilemap->activeSize.w) {
            tilemap->activeSize.w = position->x;
        }
        if (position->y > tilemap->activeSize.h) {
            tilemap->activeSize.h = position->y;
        }
    } else {
        // TODO: Handle case for shrinking active size
    }
}

void cre_tilemap_set_tile_active(CreTilemap* tilemap, SKAVector2i* position, bool isActive) {
    CreTile* tile = &tilemap->tiles[position->y][position->x];
    if (tile->isActive != isActive) {
        tile->isActive = isActive;
        cre_tilemap_update_active_size(tilemap, tile, position);
        cre_tilemap_refresh_tilemap(tilemap);
    }
}

bool cre_tilemap_is_tile_active(CreTilemap* tilemap, SKAVector2i* position) {
    return tilemap->tiles[position->y][position->x].isActive;
}

CreTileBitmask cre_tilemap_get_tile_bitmask(CreTilemap* tilemap, SKAVector2i* position) {
    return tilemap->tiles[position->y][position->x].bitmask;
}
