#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef size_t (*RBEStringHashFunc) (const char*);
typedef int (*RBEStringCompareFunc) (const char*, const char*);

typedef struct StringHashMapNode {
    struct StringHashMapNode* next;
    char* key;
    void* value;
    size_t valueSize;
} StringHashMapNode;

typedef struct RBEStringHashMap {
    size_t capacity;
    size_t size;
    RBEStringHashFunc hashFunc;
    RBEStringCompareFunc compareFunc;
    StringHashMapNode** nodes;
} RBEStringHashMap;

typedef struct RBEStringHashMapIterator {
    size_t count;
    size_t end;
    size_t index;
    StringHashMapNode* pair;
} RBEStringHashMapIterator;

// Generic hash map methods
RBEStringHashMap* rbe_string_hash_map_create(size_t capacity);
bool rbe_string_hash_map_destroy(RBEStringHashMap* hashMap);
bool rbe_string_hash_map_add(RBEStringHashMap* hashMap, const char* key, void* value, size_t valueSize);
void* rbe_string_hash_map_get(RBEStringHashMap* hashMap, const char* key);
bool rbe_string_hash_map_has(RBEStringHashMap* hashMap, const char* key);
bool rbe_string_hash_map_erase(RBEStringHashMap* hashMap, const char* key);
// Int
bool rbe_string_hash_map_add_int(RBEStringHashMap* hashMap, const char* key, int value);
int rbe_string_hash_map_get_int(RBEStringHashMap* hashMap, const char* key);
// String
bool rbe_string_hash_map_add_string(RBEStringHashMap* hashMap, const char* key, const char* value);
char* rbe_string_hash_map_get_string(RBEStringHashMap* hashMap, const char* key);

// Iterator
RBEStringHashMapIterator rbe_string_hash_map_iter_create(RBEStringHashMap* hashMap);
bool rbe_string_hash_map_iter_is_valid(RBEStringHashMap* hashMap, RBEStringHashMapIterator* iterator);
void rbe_string_hash_map_iter_advance(RBEStringHashMap* hashMap, RBEStringHashMapIterator* iterator);

#ifdef __cplusplus
}
#endif
