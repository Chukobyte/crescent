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

void spatial_hash_map_update(SESpatialHashMap* hashMap, unsigned int entity, SESpatialHashMapGridSpacesHandle* handle, SERect2* collisionRect);
bool change_cell_size_if_needed(SESpatialHashMap* hashMap, SERect2* collisionRectToCheck);
int32_t spatial_hash(SESpatialHashMap* hashMap, SEVector2* position);
void spatial_hash_map_destroy_node(SESpatialHashMap* hashMap);
SESpatialHashMapGridSpace* get_or_create_grid_space(SESpatialHashMap* hashMap, int32_t positionHash);
bool link_object_by_position_hash(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, unsigned int value, int32_t positionHash, PositionHashes* hashes);
bool unlink_object_by_entity(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, SESpatialHashMapGridSpace* gridSpace, unsigned int entity);
void unlink_all_objects_by_entity(SESpatialHashMap* hashMap, SESpatialHashMapGridSpacesHandle* object, unsigned int entity);
bool collision_result_has_entity(SESpatialHashMapCollisionResult* result, unsigned int entity);

// Public facing functions
SESpatialHashMap* se_spatial_hash_map_create(int initialCellSize) {
    SESpatialHashMap* map = SE_MEM_ALLOCATE(SESpatialHashMap);
    map->cellSize = initialCellSize;
    map->largestObjectSize = initialCellSize;
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

// The purpose of this function is to make sure that 'cellSize' is twice as big as the largest object
bool change_cell_size_if_needed(SESpatialHashMap* hashMap, SERect2* collisionRectToCheck) {
    const int objectMaxSize = collisionRectToCheck->h > collisionRectToCheck->w ? (int)collisionRectToCheck->h : (int)collisionRectToCheck->w;
    // Update largest object size of hashmap if applicable
    if (objectMaxSize > hashMap->largestObjectSize) {
        hashMap->largestObjectSize = objectMaxSize;
    }
    // Check if cell size needs to grow or shrink
    if (objectMaxSize > hashMap->cellSize * 2 || hashMap->largestObjectSize < hashMap->cellSize / 8) {
        hashMap->cellSize = objectMaxSize * 2;
        return true;
    }

    return false;
}

SESpatialHashMapGridSpacesHandle* se_spatial_hash_map_insert_or_update(SESpatialHashMap* hashMap, unsigned int entity, SERect2* collisionRect) {
    // Create new object handle if it doesn't exist
    if (!se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        SESpatialHashMapGridSpacesHandle* newHandle = SE_MEM_ALLOCATE(SESpatialHashMapGridSpacesHandle);
        newHandle->gridSpaceCount = 0;
        newHandle->collisionRect = (SERect2) {
            0.0f, 0.0f, 0.0f, 0.0f
        };
        se_hash_map_add(hashMap->objectToGridMap, &entity, &newHandle);
    }

    // Update cell size and rebuild map if an object is bigger than the cell size
    if (change_cell_size_if_needed(hashMap, collisionRect)) {
        // Since we have changed cell size and largest object size, rebuild spatial hash
        SE_HASH_MAP_FOR_EACH(hashMap->objectToGridMap, iter) {
            const unsigned int updatedEntity = *(unsigned int*) iter.pair->key;
            if (entity != updatedEntity) {
                SESpatialHashMapGridSpacesHandle* updatedHandle = (SESpatialHashMapGridSpacesHandle*) iter.pair->value;
                spatial_hash_map_update(hashMap, updatedEntity, updatedHandle, &updatedHandle->collisionRect);
            }
        }
    }

    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    spatial_hash_map_update(hashMap, entity, objectHandle, collisionRect);
    return objectHandle;
}

void spatial_hash_map_update(SESpatialHashMap* hashMap, unsigned int entity, SESpatialHashMapGridSpacesHandle* handle, SERect2* collisionRect) {
    memcpy(&handle->collisionRect, collisionRect, sizeof(SERect2));

    // Unlink all previous spaces and objects
    unlink_all_objects_by_entity(hashMap, handle, entity);

    // Add values to spaces and spaces to object handles (moving clockwise starting from top-left)
    PositionHashes hashes = { .hashCount = 0 };
    // Top left
    const int32_t topLeftHash = spatial_hash(hashMap, &(SEVector2) {
        collisionRect->x, collisionRect->y
    });
    link_object_by_position_hash(hashMap, handle, entity, topLeftHash, &hashes);
    // Top right
    const int32_t topRightHash = spatial_hash(hashMap, &(SEVector2) {
        collisionRect->x + collisionRect->w, collisionRect->y
    });
    link_object_by_position_hash(hashMap, handle, entity, topRightHash, &hashes);
    // Bottom Left
    const int32_t bottomLeftHash = spatial_hash(hashMap, &(SEVector2) {
        collisionRect->x, collisionRect->y + collisionRect->h
    });
    link_object_by_position_hash(hashMap, handle, entity, bottomLeftHash, &hashes);
    // Bottom Right
    const int32_t bottomRightHash = spatial_hash(hashMap, &(SEVector2) {
        collisionRect->x + collisionRect->w, collisionRect->y + collisionRect->h
    });
    link_object_by_position_hash(hashMap, handle, entity, bottomRightHash, &hashes);
}

void se_spatial_hash_map_remove(SESpatialHashMap* hashMap, unsigned int entity) {
    if (!se_hash_map_has(hashMap->objectToGridMap, &entity)) {
        return;
    }
    SESpatialHashMapGridSpacesHandle* objectHandle = (SESpatialHashMapGridSpacesHandle*) *(SESpatialHashMapGridSpacesHandle**) se_hash_map_get(hashMap->objectToGridMap, &entity);
    unlink_all_objects_by_entity(hashMap, objectHandle, entity);
    se_hash_map_erase(hashMap->objectToGridMap, &entity);
    // TODO: Use something more efficient than looping through the entire hashmap to find the largest object size
    const int MaxObjectSize = objectHandle->collisionRect.h > objectHandle->collisionRect.w ? (int)objectHandle->collisionRect.h : (int)objectHandle->collisionRect.w;
    if (MaxObjectSize == hashMap->largestObjectSize) {
        int foundLargestObjectSize = -1;
        SE_HASH_MAP_FOR_EACH(hashMap->objectToGridMap, iter) {
            SESpatialHashMapGridSpacesHandle* nodeObjectHandle = (SESpatialHashMapGridSpacesHandle*) iter.pair->value;
            const int nodeMaxObjectSize = nodeObjectHandle->collisionRect.h > nodeObjectHandle->collisionRect.w ? (int)nodeObjectHandle->collisionRect.h : (int)nodeObjectHandle->collisionRect.w;
            // Early out if we find another object with the same size
            if (nodeMaxObjectSize == hashMap->largestObjectSize) {
                foundLargestObjectSize = -1;
                break;
            } else if(nodeMaxObjectSize > foundLargestObjectSize) {
                foundLargestObjectSize = nodeMaxObjectSize;
            }
        }
        if (foundLargestObjectSize > 0) {
            hashMap->largestObjectSize = foundLargestObjectSize;
        }
    }
    SE_MEM_FREE(objectHandle);
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
int32_t spatial_hash(SESpatialHashMap* hashMap, SEVector2* position) {
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
