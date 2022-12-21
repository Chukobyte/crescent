#include "se_spatial_hash_map.h"

#include <stdint.h>
#include <string.h>

#include "../memory/se_mem.h"
#include "../utils/se_assert.h"

#define SE_SPATIAL_HASH_NULL_ENTITY 4294967295
#define SE_SPATIAL_HASH_MAX_POSITION_HASH 8

typedef struct PositionHashes {
    size_t hashCount;
    int32_t hashes[SE_SPATIAL_HASH_MAX_POSITION_HASH];
} PositionHashes;

int32_t spatial_hash(SESpatialHashMap* hashMap, Vector2* position);
void spatial_hash_map_destroy_node(SESpatialHashMap* hashMap);
SESpatialHashMapGridSpace* get_or_create_grid_space(SESpatialHashMap* hashMap, int32_t positionHash);
bool link_object_by_position_hash(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, unsigned int value, int32_t positionHash, PositionHashes* hashes);
bool unlink_object_by_entity(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, SESpatialHashMapGridSpace* gridSpace, unsigned int entity);
void unlink_all_objects_by_entity(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, unsigned int entity);
bool collision_result_has_entity(SESpatialHashMapCollisionResult* result, unsigned int entity);

// Public facing functions
SESpatialHashMap* se_spatial_hash_map_create(int cellSize) {
    SESpatialHashMap* map = SE_MEM_ALLOCATE(SESpatialHashMap);
    map->cellSize = cellSize;
    map->gridMap = se_hash_map_create(sizeof(int32_t), sizeof(SESpatialHashMapGridSpace**), SE_HASH_MAP_MIN_CAPACITY);
    map->objectToGridMap = se_hash_map_create(sizeof(unsigned int), sizeof(SESpatialHashMapGridSpacesHandle**), SE_HASH_MAP_MIN_CAPACITY);
    map->doesCollisionDataNeedUpdating = false;
    return map;
}

void se_spatial_hash_map_destroy(SESpatialHashMap* hashMap) {
    // Grid map
    SE_HASH_MAP_FOR_EACH(hashMap->gridMap, iter) {
        HashMapNode* node = iter.pair;
        SESpatialHashMapGridSpace* gridSpace = (SESpatialHashMapGridSpace*) *(SESpatialHashMapGridSpace**) node->value;
        SE_MEM_FREE(gridSpace);
    }
    se_hash_map_destroy(hashMap->gridMap);
    // Grid space Handle Map
    SE_HASH_MAP_FOR_EACH(hashMap->objectToGridMap, iter) {
        HashMapNode* node = iter.pair;
        SESpatialHashMapGridSpacesHandle* handle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) node->value;
        SE_MEM_FREE(handle);
    }
    se_hash_map_destroy(hashMap->objectToGridMap);
    // Finally free the hashmap memory
    SE_MEM_FREE(hashMap);
}

SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_insert_or_update(SESpatialHashMap* hashMap, unsigned int entity, Rect2* collisionRect) {
    // Create new object handle if it doesn't exist
    if (!se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        SESpatialHashMapGridSpacesHandle* newHandle = SE_MEM_ALLOCATE(SESpatialHashMapGridSpacesHandle);
        newHandle->gridSpaceCount = 0;
        se_hash_map_add(hashMap->objectToGridMap, &entity, &newHandle);
    }
    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    memcpy(&objectHandle->collisionRect, collisionRect, sizeof(Rect2));

    // Unlink all previous spaces and objects
    unlink_all_objects_by_entity(hashMap, objectHandle, entity);

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
    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    unlink_all_objects_by_entity(hashMap, objectHandle, entity);
    SE_MEM_FREE(objectHandle);
    se_hash_map_erase(hashMap->objectToGridMap, &entity);
}

SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_get(SESpatialHashMap* hashMap, unsigned int entity) {
    if (se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        return (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    }
    return NULL;
}

SESpatialHashMapCollisionResult se_spatial_hash_map_compute_collision(SESpatialHashMap* hashMap, unsigned int entity) {
    SESpatialHashMapCollisionResult result = { .collisionCount = 0 };
    void* objectHandlePtr = se_hash_map_get(hashMap->objectToGridMap, &entity);
    // Early out if object not in spatial hash map
    if (objectHandlePtr == NULL) {
        return result;
    }
    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) objectHandlePtr;
    for (size_t i = 0; i < objectHandle->gridSpaceCount; i++) {
        SESpatialHashMapGridSpace* gridSpace = objectHandle->gridSpaces[i];
        for (size_t j = 0; j < gridSpace->entityCount; j++) {
            unsigned int entityToCollide = gridSpace->entities[j];
            if (entity != entityToCollide && !collision_result_has_entity(&result, entityToCollide)) {
                SESpatialHashMapGridSpacesHandle* entityToCollideObjectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entityToCollide);
                // Now that we have passed all checks, actually check collision
                if (se_rect2_does_rectangles_overlap(&objectHandle->collisionRect, &entityToCollideObjectHandle->collisionRect)) {
                    SE_ASSERT_FMT(result.collisionCount + 1 <= SE_SPATIAL_HASH_GRID_MAX_COLLISIONS, "At limit of collisions '%d', consider increasing 'SE_SPATIAL_HASH_GRID_MAX_COLLISIONS'", SE_SPATIAL_HASH_GRID_MAX_COLLISIONS);
                    result.collisions[result.collisionCount++] = entityToCollide;
                }
            }
        }
    }
    return result;
}

// Internal Functions
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
    SE_ASSERT_FMT(hashes->hashCount + 1 < SE_SPATIAL_HASH_MAX_POSITION_HASH,
                  "Current hash count for value '%d' exceeds 'SE_SPATIAL_HASH_MAX_POSITION_HASH (%d)', consider increasing SE_SPATIAL_HASH_MAX_POSITION_HASH!");
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

void unlink_all_objects_by_entity(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, unsigned int entity) {
    const size_t numberOfSpaces = object->gridSpaceCount;
    for (size_t i = 0; i < numberOfSpaces; i++) {
        unlink_object_by_entity(hashMap, object, object->gridSpaces[i], entity);
    }
}

bool collision_result_has_entity(SESpatialHashMapCollisionResult* result, unsigned int entity) {
    for (size_t i = 0; i < result->collisionCount; i++) {
        if (entity == result->collisions[i]) {
            return true;
        }
    }
    return false;
}
