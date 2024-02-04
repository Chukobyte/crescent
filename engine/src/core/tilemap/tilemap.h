#pragma once

#include <seika/math/se_math.h>

#define CRE_TILEMAP_MAX_ROWS 32
#define CRE_TILEMAP_MAX_COLS 32

// Can determine which tiles are adjacent to the current one, all active tiles will have a center
typedef enum CreTileBitmask {
    CreTileType_INVALID = 0,
    CreTileType_CENTER = 1 << 0,
    CreTileType_TOP = 1 << 1,
    CreTileType_RIGHT = 1 << 2,
    CreTileType_BOTTOM = 1 << 3,
    CreTileType_LEFT = 1 << 4,
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

#define CRE_TILEMAP_DEFAULT_EMPTY SKA_STRUCT_LITERAL(CreTilemap){ .activeSize = 0, .tiles = {{0}} }

void cre_tilemap_set_tile_active(CreTilemap* tilemap, const SKAVector2i* position, bool isActive);
bool cre_tilemap_is_tile_active(const CreTilemap* tilemap, const SKAVector2i* position);
CreTileBitmask cre_tilemap_get_tile_bitmask(const CreTilemap* tilemap, const SKAVector2i* position);
