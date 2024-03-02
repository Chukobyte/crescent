#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/data_structures/ska_array2d.h>
#include <seika/data_structures/ska_array_list.h>

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

typedef struct CreTileData {
    bool isActive;
    CreTileBitmask bitmask;
    SKAVector2i renderCoords; // Render coordinates used to draw from a texture (tileset)
    SKAVector2i position; // Render coordinates used to draw from a texture (tileset)
} CreTileData;

typedef enum CreTilemapAutotileBitmaskMode {
    CreTilemapBitmaskMode_3x3_MINIMAL = 0,
    CreTilemapBitmaskMode_2x2 = 1,
    CreTilemapBitmaskMode_3x3 = 2,
} CreTilemapAutotileBitmaskMode;

typedef struct CreTileset {
    struct SETexture* texture;
    SKASize2Di tileSize;
} CreTileset;

typedef struct CreTilemapTransactionItem {
    CreTileData data;
    struct CreTilemapTransactionItem* next;
} CreTilemapTransactionItem;

// Transaction for enabling/disable tiles, used to prevent updating and resizing every event
typedef struct CreTilemapTransaction {
    CreTilemapTransactionItem* rootItem;
    SKASize2Di requestedSize;
} CreTilemapTransaction;

typedef struct CreTilemap {
    CreTileset tileset;
    SkaArrayList* activeTiles;
    CreTilemapAutotileBitmaskMode bitmaskMode; // TODO: Actually implement multiple bitmask modes (instead of just default to 3x3 Minimal)
    SkaArray2D* tilesArray; // Contains tile data
    CreTilemapTransaction* activeTransaction;
} CreTilemap;

#define CRE_TILEMAP_DEFAULT_EMPTY SKA_STRUCT_LITERAL(CreTilemap){ \
.tileset = (CreTileset){ .texture = NULL, .tileSize = (SKASize2Di){ 32, 32 } }, \
.activeTiles = NULL, \
.bitmaskMode = CreTilemapBitmaskMode_3x3_MINIMAL, \
.tilesArray = NULL, \
.activeTransaction = NULL \
}

void cre_tilemap_initialize(CreTilemap* tilemap);
CreTilemap* cre_tilemap_create_and_initialize();
void cre_tilemap_finalize(CreTilemap* tilemap);
void cre_tilemap_set_tile_active(CreTilemap* tilemap, const SKAVector2i* position, bool isActive);
void cre_tilemap_set_tile_render_coord(CreTilemap* tilemap, const SKAVector2i* position, const SKAVector2i* coord);
void cre_tilemap_set_tile_data(CreTilemap* tilemap, const CreTileData* tileData);
bool cre_tilemap_is_tile_active(const CreTilemap* tilemap, const SKAVector2i* position);
void cre_tilemap_clear_all_tiles(const CreTilemap* tilemap);
// Should be called once finished setting active tiles
void cre_tilemap_commit_active_tile_changes(CreTilemap* tilemap);
void cre_tilemap_clear_active_tile_changes(CreTilemap* tilemap);
CreTileBitmask cre_tilemap_get_tile_bitmask(const CreTilemap* tilemap, const SKAVector2i* position);
const CreTileData* cre_tilemap_get_tile_data(const CreTilemap* tilemap, const SKAVector2i* position);

#ifdef __cplusplus
}
#endif
