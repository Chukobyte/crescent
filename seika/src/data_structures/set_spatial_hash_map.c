#include "set_spatial_hash_map.h"

#include <stdint.h>

#include "../memory/se_mem.h"

#define SE_SPATIAL_HASH_GRID_SPACE_LIMIT 1000
#define SE_SPATIAL_HASH_NULL_ENTITY 4294967295

//typedef struct SESpatialHashMapObject {
//
//} SESpatialHashMapObject;

typedef struct PositionHashes {
    size_t hashCount;
    int32_t hashes[4];
} PositionHashes;

SESpatialHashMap* se_spatial_hash_map_create(int cellSize) {
    SESpatialHashMap* map = SE_MEM_ALLOCATE(SESpatialHashMap);
    map->cellSize = cellSize;
    map->gridMap = se_hash_map_create(sizeof(int32_t*), sizeof(SESpatialHashMapGridSpace**), SE_HASH_MAP_MIN_CAPACITY);
    map->objectToGridMap = se_hash_map_create(sizeof(unsigned int*), sizeof(SESpatialHashMapGridSpacesHandle**), SE_HASH_MAP_MIN_CAPACITY);
    map->doesCollisionDataNeedUpdating = false;
    return map;
}

void se_spatial_hash_map_destroy(SESpatialHashMap* hashMap) {
    // TODO: Destroy internal data
    se_hash_map_destroy(hashMap->gridMap);
    se_hash_map_destroy(hashMap->objectToGridMap);
    SE_MEM_FREE(hashMap);
}

int32_t spatial_hash(SESpatialHashMap* hashMap, Vector2* position) {
    const int32_t x = (int32_t) position->x / hashMap->cellSize;
    const int32_t y = (int32_t) position->y / hashMap->cellSize;
    const int32_t hash = ((x * x) ^ (y * y)) % INT32_MAX;
    return hash;
}

SESpatialHashMapGridSpace* get_or_create_grid_space(SESpatialHashMap* hashMap, int32_t positionHash) {
    if (!se_hash_map_has(hashMap->gridMap, &positionHash)) {
        SESpatialHashMapGridSpace* newGridSpace = SE_MEM_ALLOCATE(SESpatialHashMapGridSpace);
        newGridSpace->entityCount = 0;
        se_hash_map_add(hashMap->gridMap, &positionHash, &newGridSpace);
    }
    SESpatialHashMapGridSpace* gridSpace = (SESpatialHashMapGridSpace*) *(SESpatialHashMapGridSpace**) se_hash_map_get(hashMap->gridMap, &positionHash);
    return gridSpace;
}

bool link_object_by_position_hash(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, unsigned int value, int32_t positionHash, PositionHashes* hashes) {
    // Exit if position hash exists
    for (size_t i = 0; i < hashes->hashCount; i++) {
        if (positionHash == hashes->hashes[i]) {
            return false;
        }
    }
    // Link
    SESpatialHashMapGridSpace* gridSpace = get_or_create_grid_space(hashMap, positionHash);
    gridSpace->entities[gridSpace->entityCount++] = value;
    object->gridSpaces[object->gridSpaceCount++] = gridSpace;
    hashes->hashes[hashes->hashCount++] = positionHash;
    return true;
}

bool unlink_object_by_entity(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, SESpatialHashMapGridSpace* gridSpace, unsigned int entity) {
    bool objectUnlinked = false;
    for (size_t i = 0; i < gridSpace->entityCount; i++) {
        if (entity == gridSpace->entities[i] || gridSpace->entities[i] == SE_SPATIAL_HASH_NULL_ENTITY) {
            if (i + 1 < gridSpace->entityCount) {
                gridSpace->entities[i] = gridSpace->entities[i + 1];
                gridSpace->entities[i + 1] = SE_SPATIAL_HASH_NULL_ENTITY;
            }
            objectUnlinked = true;
        }
    }
    if (objectUnlinked) {
        object->gridSpaceCount--;
        gridSpace->entityCount--;
    }
    return objectUnlinked;
}

SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_insert(SESpatialHashMap* hashMap, unsigned int entity, Rect2* collisionRect) {
    // Create new object handle
    if (!se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        SESpatialHashMapGridSpacesHandle* newHandle = SE_MEM_ALLOCATE(SESpatialHashMapGridSpacesHandle);
        newHandle->gridSpaceCount = 0;
        se_hash_map_add(hashMap->objectToGridMap, &entity, &newHandle);
    }
    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    objectHandle->gridSpaceCount = 0;

    // Add values to spaces and spaces to object handles (moving clockwise starting from top-left)
    PositionHashes hashes = { .hashCount = 0 };
    // Top left
    const int32_t topLeftHash = spatial_hash(hashMap, &(Vector2) {
        collisionRect->x, collisionRect->y
    });
    link_object_by_position_hash(hashMap, objectHandle, entity, topLeftHash, &hashes);
    // Top right
    const int32_t topRightHash = spatial_hash(hashMap, &(Vector2) {
        collisionRect->x + collisionRect->w, collisionRect->y
    });
    link_object_by_position_hash(hashMap, objectHandle, entity, topRightHash, &hashes);
    // Bottom Left
    const int32_t bottomLeftHash = spatial_hash(hashMap, &(Vector2) {
        collisionRect->x, collisionRect->y + collisionRect->h
    });
    link_object_by_position_hash(hashMap, objectHandle, entity, bottomLeftHash, &hashes);
    // Bottom Right
    const int32_t bottomRightHash = spatial_hash(hashMap, &(Vector2) {
        collisionRect->x + collisionRect->w, collisionRect->y + collisionRect->h
    });
    link_object_by_position_hash(hashMap, objectHandle, entity, bottomRightHash, &hashes);

    return objectHandle;
}

void se_spatial_hash_map_remove(SESpatialHashMap* hashMap, unsigned int entity) {
    if (!se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        return;
    }
    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) (SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    const size_t numberOfSpaces = objectHandle->gridSpaceCount;
    for (size_t i = 0; i < numberOfSpaces; i++) {
        unlink_object_by_entity(hashMap, objectHandle, objectHandle->gridSpaces[i], entity);
    }
    SE_MEM_FREE(objectHandle);
    se_hash_map_erase(hashMap->objectToGridMap, &entity);
}

SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_get(SESpatialHashMap* hashMap, unsigned int entity) {
    if (se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        return (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    }
    return NULL;
}

// Clean up unused spaces
void se_spatial_hash_map_sanitize() {}

void se_spatial_hash_map_update_object(SESpatialHashMapGridSpacesHandle* object, Rect2* collisionRect) {}

void se_spatial_hash_map_process_collisions(SESpatialHashMap* hashMap) {
    if (!hashMap->doesCollisionDataNeedUpdating) {
        return;
    }
    hashMap->doesCollisionDataNeedUpdating = false;
}
