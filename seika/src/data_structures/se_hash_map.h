#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

#define SE_HASH_MAP_SHRINK_THRESHOLD 0.25f
#define SE_HASH_MAP_MIN_CAPACITY 8

#define SE_HASH_MAP_FOR_EACH(HASH_MAP, ITER_NAME) \
for (SEHashMapIterator ITER_NAME = se_hash_map_iter_create(HASH_MAP); se_hash_map_iter_is_valid(HASH_MAP, &(ITER_NAME)); se_hash_map_iter_advance(HASH_MAP, &(ITER_NAME)))

typedef size_t (*SEHashFunc) (void*, size_t);
typedef int (*SECompareFunc) (void*, void*, size_t);

typedef struct HashMapNode {
    struct HashMapNode* next;
    void* key;
    void* value;
} HashMapNode;

typedef struct SEHashMap {
    size_t keySize;
    size_t valueSize;
    size_t capacity;
    size_t size;
    SEHashFunc hashFunc;
    SECompareFunc compareFunc;
    HashMapNode** nodes;
} SEHashMap;

typedef struct SEHashMapIterator {
    size_t count;
    size_t end;
    size_t index;
    HashMapNode* pair;
} SEHashMapIterator;

// Generic hash map methods
SEHashMap* se_hash_map_create(size_t keySize, size_t valueSize, size_t capacity);
bool se_hash_map_destroy(SEHashMap* hashMap);
bool se_hash_map_add(SEHashMap* hashMap, void* key, void* value);
void* se_hash_map_get(SEHashMap* hashMap, void* key);
bool se_hash_map_has(SEHashMap* hashMap, void* key);
bool se_hash_map_erase(SEHashMap* hashMap, void* key);

// Iterator
SEHashMapIterator se_hash_map_iter_create(SEHashMap* hashMap);
bool se_hash_map_iter_is_valid(SEHashMap* hashMap, SEHashMapIterator* iterator);
void se_hash_map_iter_advance(SEHashMap* hashMap, SEHashMapIterator* iterator);

#ifdef __cplusplus
}
#endif
