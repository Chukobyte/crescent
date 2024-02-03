#pragma once

#include <seika/math/se_math.h>

#define CRE_TILEMAP_MAX_ROWS 32
#define CRE_TILEMAP_MAX_COLS 32

typedef enum CreTileBitmask {
    CreTileType_INVALID = 0,
    CreTileType_TOP = 1 << 0,
    CreTileType_RIGHT = 1 << 1,
    CreTileType_BOTTOM = 1 << 2,
    CreTileType_LEFT = 1 << 3,
    CreTileType_CENTER = 1 << 4,
} CreTileBitmask;

typedef struct CreTile {
    bool isActive;
    CreTileBitmask bitmask;
} CreTile;

typedef struct CreTileData {
    CreTile* tile;
    SKAVector2i position;
} CreTileData;

typedef struct CreTilemap {
    SKASize2Di activeSize;
    CreTile tiles[CRE_TILEMAP_MAX_ROWS][CRE_TILEMAP_MAX_COLS];
} CreTilemap;

void cre_tilemap_set_tile_active(CreTilemap* tilemap, SKAVector2i* position, bool isActive);
bool cre_tilemap_is_tile_active(CreTilemap* tilemap, SKAVector2i* position);
CreTileBitmask cre_tilemap_get_tile_bitmask(CreTilemap* tilemap, SKAVector2i* position);
