#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef size_t (*RBEHashFunc) (void*, size_t);
typedef int (*RBECompareFunc) (void*, void*, size_t);

typedef struct HashMapNode {
    struct HashMapNode* next;
    void* key;
    void* value;
} HashMapNode;

typedef struct RBEHashMap {
    size_t keySize;
    size_t valueSize;
    size_t capacity;
    size_t size;
    RBEHashFunc hashFunc;
    RBECompareFunc compareFunc;
    HashMapNode** nodes;
} RBEHashMap;

typedef struct RBEHashMapIterator {
    size_t count;
    size_t end;
    size_t index;
    HashMapNode* pair;
} RBEHashMapIterator;

// Generic hash map methods
RBEHashMap* rbe_hash_map_create(size_t keySize, size_t valueSize, size_t capacity);
bool rbe_hash_map_destroy(RBEHashMap* hashMap);
bool rbe_hash_map_add(RBEHashMap* hashMap, void* key, void* value);
void* rbe_hash_map_get(RBEHashMap* hashMap, void* key);
bool rbe_hash_map_has(RBEHashMap* hashMap, void* key);
bool rbe_hash_map_erase(RBEHashMap* hashMap, void* key);

// Iterator
RBEHashMapIterator rbe_hash_map_iter_create(RBEHashMap* hashMap);
bool rbe_hash_map_iter_is_valid(RBEHashMap* hashMap, RBEHashMapIterator* iterator);
void rbe_hash_map_iter_advance(RBEHashMap* hashMap, RBEHashMapIterator* iterator);

#ifdef __cplusplus
}
#endif
