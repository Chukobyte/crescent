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

// Generic hash map methods
RBEHashMap* rbe_hash_map_create(size_t keySize, size_t valueSize, size_t capacity);
bool rbe_hash_map_destroy(RBEHashMap* hashMap);
bool rbe_hash_map_add(RBEHashMap* hashMap, void* key, void* value);
void* rbe_hash_map_get(RBEHashMap* hashMap, void* key);
bool rbe_hash_map_has(RBEHashMap* hashMap, void* key);
bool rbe_hash_map_erase(RBEHashMap* hashMap, void* key);
// String key methods, creates temp string dynamically for key
// TODO: Check performance and stuff.  If it's bash, don't use string key version or create separate api.
bool rbe_hash_map_add_from_string_key(RBEHashMap* hashMap, const char* key, void* value);
void* rbe_hash_map_get_from_string_key(RBEHashMap* hashMap, const char* key);
bool rbe_hash_map_has_from_string_key(RBEHashMap* hashMap, const char* key);
bool rbe_hash_map_erase_from_string_key(RBEHashMap* hashMap, const char* key);
// String key and string value methods
bool rbe_hash_map_add_from_string_key_and_value(RBEHashMap* hashMap, const char* key, const char* value);

#ifdef __cplusplus
}
#endif
