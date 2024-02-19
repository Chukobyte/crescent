#pragma once

#include <seika/math/se_math.h>

#define CRE_TILEMAP_MAX_ROWS 32
#define CRE_TILEMAP_MAX_COLS 32

struct SETexture;

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

typedef struct CreTileArray2D {
    SKASize2Di size; // w = cols, h = rows
    CreTile** array;
} CreTileArray2D;

typedef enum CreTilemapBitmaskMode {
    CreTilemapBitmaskMode_3x3_MINIMAL = 0,
    CreTilemapBitmaskMode_2x2 = 1,
    CreTilemapBitmaskMode_3x3 = 2,
} CreTilemapBitmaskMode;

typedef struct CreTileset {
    struct SETexture* texture;
    SKASize2Di tileSize;
} CreTileset;

typedef struct CreTilemap {
    CreTileset tileset;
    CreTilemapBitmaskMode bitmaskMode; // TODO: Actually implement multiple bitmask modes (instead of just default to 3x3 Minimal)
    SKASize2Di activeSize;
    CreTileArray2D tiles;
//    CreTile tiles[CRE_TILEMAP_MAX_ROWS][CRE_TILEMAP_MAX_COLS];
} CreTilemap;

#define CRE_TILEMAP_DEFAULT_EMPTY SKA_STRUCT_LITERAL(CreTilemap){ \
.activeSize = 0, \
.bitmaskMode = CreTilemapBitmaskMode_3x3_MINIMAL, \
.tiles = { .size = { 32, 32 }, .array = NULL } \
}

void cre_tile_array2d_allocate(CreTileArray2D* tileArray);
void cre_tile_array2d_deallocate(CreTileArray2D* tileArray);

void cre_tilemap_set_tile_active(CreTilemap* tilemap, const SKAVector2i* position, bool isActive);
bool cre_tilemap_is_tile_active(const CreTilemap* tilemap, const SKAVector2i* position);
CreTileBitmask cre_tilemap_get_tile_bitmask(const CreTilemap* tilemap, const SKAVector2i* position);
