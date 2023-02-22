#pragma once

#include "../math/se_math.h"
#include "se_hash_map.h"

#define SE_SPATIAL_HASH_GRID_SPACE_ENTITY_LIMIT 32
#define SE_SPATIAL_HASH_GRID_MAX_COLLISIONS 8

// Note: Spatial hash expects rectangles that have 0 rotation

// Contains the object id for a particular grid space
typedef struct SESpatialHashMapGridSpace {
    unsigned int entities[SE_SPATIAL_HASH_GRID_SPACE_ENTITY_LIMIT];
    size_t entityCount;
} SESpatialHashMapGridSpace;

// Contains all grid spaces an object is assigned to
typedef struct SESpatialHashMapGridSpacesHandle {
    size_t gridSpaceCount;
    SERect2 collisionRect;
    SESpatialHashMapGridSpace* gridSpaces[4];
} SESpatialHashMapGridSpacesHandle;

// Contains a hash map of buckets that correspond to spaces on a grid
typedef struct SESpatialHashMap {
    int cellSize;  // Cell size should be 2x the size of the largest object
    SEHashMap* gridMap; // Contains objects within a grid.
    SEHashMap* objectToGridMap; // Contains contains all grid spaces an object is assigned to.
    bool doesCollisionDataNeedUpdating;
} SESpatialHashMap;

typedef struct SESpatialHashMapCollisionResult {
    size_t collisionCount;
    unsigned int collisions[SE_SPATIAL_HASH_GRID_MAX_COLLISIONS];
} SESpatialHashMapCollisionResult;

SESpatialHashMap* se_spatial_hash_map_create(int cellSize);
void se_spatial_hash_map_destroy(SESpatialHashMap* hashMap);
SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_insert_or_update(SESpatialHashMap* hashMap, unsigned int entity, SERect2* collisionRect);
void se_spatial_hash_map_remove(SESpatialHashMap* hashMap, unsigned int entity);
SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_get(SESpatialHashMap* hashMap, unsigned int entity);
SESpatialHashMapCollisionResult se_spatial_hash_map_compute_collision(SESpatialHashMap* hashMap, unsigned int entity);
