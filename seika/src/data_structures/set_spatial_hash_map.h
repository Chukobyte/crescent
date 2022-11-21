#pragma once

#include "../math/se_math.h"
#include "se_hash_map.h"

// Contains the object id for a particular grid space
typedef struct SESpatialHashMapGridSpace {
    unsigned int entities[16];
    size_t entityCount;
} SESpatialHashMapGridSpace;

// Contains all grid spaces an object is assigned to
typedef struct SESpatialHashMapGridSpacesHandle {
    SESpatialHashMapGridSpace* gridSpaces[4];
    size_t gridSpaceCount;
} SESpatialHashMapGridSpacesHandle;

// Contains a hash map of buckets that correspond to spaces on a grid
typedef struct SESpatialHashMap {
    int cellSize;  // Cell size should be 2x the size of the largest object
    SEHashMap* gridMap; // Contains objects within a grid.
    SEHashMap* objectToGridMap; // Contains contains all grid spaces an object is assigned to.
    bool doesCollisionDataNeedUpdating;
} SESpatialHashMap;

SESpatialHashMap* se_spatial_hash_map_create(int cellSize);
void se_spatial_hash_map_destroy(SESpatialHashMap* hashMap);
SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_insert(SESpatialHashMap* hashMap, unsigned int entity, Rect2* collisionRect);
void se_spatial_hash_map_remove(SESpatialHashMap* hashMap, unsigned int entity);
SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_get(SESpatialHashMap* hashMap, unsigned int entity);
