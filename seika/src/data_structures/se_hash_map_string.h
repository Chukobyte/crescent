#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

#define SE_STRING_HASH_MAP_SHRINK_THRESHOLD 0.25f
#define SE_STRING_HASH_MAP_MIN_CAPACITY 8

#define SE_STRING_HASH_MAP_FOR_EACH(HASH_MAP, ITER_NAME) \
for (SEStringHashMapIterator ITER_NAME = se_string_hash_map_iter_create(HASH_MAP); se_string_hash_map_iter_is_valid(HASH_MAP, &(ITER_NAME)); se_string_hash_map_iter_advance(HASH_MAP, &(ITER_NAME)))

typedef size_t (*SEStringHashFunc) (const char*);
typedef int (*SEStringCompareFunc) (const char*, const char*);

typedef struct StringHashMapNode {
    struct StringHashMapNode* next;
    char* key;
    void* value;
    size_t valueSize;
} StringHashMapNode;

typedef struct SEStringHashMap {
    size_t capacity;
    size_t size;
    SEStringHashFunc hashFunc;
    SEStringCompareFunc compareFunc;
    StringHashMapNode** nodes;
} SEStringHashMap;

typedef struct SEStringHashMapIterator {
    size_t count;
    size_t end;
    size_t index;
    StringHashMapNode* pair;
} SEStringHashMapIterator;

// Generic hash map methods
SEStringHashMap* se_string_hash_map_create(size_t capacity);
SEStringHashMap* se_string_hash_map_create_default_capacity();
bool se_string_hash_map_destroy(SEStringHashMap* hashMap);
bool se_string_hash_map_add(SEStringHashMap* hashMap, const char* key, void* value, size_t valueSize);
void* se_string_hash_map_get(SEStringHashMap* hashMap, const char* key);
bool se_string_hash_map_has(SEStringHashMap* hashMap, const char* key);
bool se_string_hash_map_erase(SEStringHashMap* hashMap, const char* key);
// Int
bool se_string_hash_map_add_int(SEStringHashMap* hashMap, const char* key, int value);
int se_string_hash_map_get_int(SEStringHashMap* hashMap, const char* key);
// String
bool se_string_hash_map_add_string(SEStringHashMap* hashMap, const char* key, const char* value);
char* se_string_hash_map_get_string(SEStringHashMap* hashMap, const char* key);

// Iterator
SEStringHashMapIterator se_string_hash_map_iter_create(SEStringHashMap* hashMap);
bool se_string_hash_map_iter_is_valid(SEStringHashMap* hashMap, SEStringHashMapIterator* iterator);
void se_string_hash_map_iter_advance(SEStringHashMap* hashMap, SEStringHashMapIterator* iterator);

#ifdef __cplusplus
}
#endif
