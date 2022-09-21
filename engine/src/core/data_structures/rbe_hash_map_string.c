#include "rbe_hash_map_string.h"

#include <string.h>

#include "../memory/rbe_mem.h"
#include "../utils/rbe_string_util.h"
#include "../utils/rbe_assert.h"

size_t rbe_default_hash_string(const char* raw_key);
int rbe_default_compare_string(const char* first_key, const char* second_key);

StringHashMapNode* hash_map_create_node_string(RBEStringHashMap* hashMap, const char* key, void* value, size_t valueSize, StringHashMapNode* next);
void hash_map_destroy_node_string(StringHashMapNode* node);

bool hash_map_push_front_string(RBEStringHashMap* hashMap, size_t index, const char* key, void* value, size_t valueSize);
void string_hash_map_grow_if_needed(RBEStringHashMap* hashMap);
void string_hash_map_shrink_if_needed(RBEStringHashMap* hashMap);
void string_hash_map_allocate(RBEStringHashMap* hashMap, size_t capacity);
void string_hash_map_rehash(RBEStringHashMap* hashMap, StringHashMapNode** oldNode, size_t oldCapacity);
void string_hash_map_resize(RBEStringHashMap* hashMap, size_t newCapacity);

RBEStringHashMap* rbe_string_hash_map_create(size_t capacity) {
    RBEStringHashMap* map = (RBEStringHashMap*) RBE_MEM_ALLOCATE(RBEStringHashMap);
    map->capacity = capacity;
    map->size = 0;
    map->hashFunc = rbe_default_hash_string;
    map->compareFunc = rbe_default_compare_string;
    map->nodes = (StringHashMapNode**) RBE_MEM_ALLOCATE_SIZE(capacity * sizeof(StringHashMapNode*));
    memset(map->nodes, 0, capacity * sizeof(StringHashMapNode*)); // TODO: fix
    return map;
}

StringHashMapNode* hash_map_create_node_string(RBEStringHashMap* hashMap, const char* key, void* value, size_t valueSize, StringHashMapNode* next) {
    StringHashMapNode* node = (StringHashMapNode*) RBE_MEM_ALLOCATE_SIZE(sizeof(StringHashMapNode));
    node->key = rbe_strdup(key);
    node->value = RBE_MEM_ALLOCATE_SIZE(valueSize);
    memcpy(node->value, value, valueSize);
    node->valueSize = valueSize;
    node->next = next;
    return node;
}

bool rbe_string_hash_map_destroy(RBEStringHashMap* hashMap) {
    StringHashMapNode* node = NULL;
    StringHashMapNode* next = NULL;
    for (size_t chain = 0; chain < hashMap->capacity; chain++) {
        node = hashMap->nodes[chain];
        while (node) {
            next = node->next;
            hash_map_destroy_node_string(node);
            node = next;
        }
    }

    RBE_MEM_FREE(hashMap);

    return true;
}

bool hash_map_push_front_string(RBEStringHashMap* hashMap, size_t index, const char* key, void* value, size_t valueSize) {
    hashMap->nodes[index] = hash_map_create_node_string(hashMap, key, value, valueSize, hashMap->nodes[index]);
    return hashMap->nodes[index] != NULL;
}

bool rbe_string_hash_map_add(RBEStringHashMap* hashMap, const char* key, void* value, size_t valueSize) {
    RBE_ASSERT(hashMap != NULL);
    RBE_ASSERT(key != NULL);
    RBE_ASSERT(value != NULL);

    string_hash_map_grow_if_needed(hashMap);

    size_t index = hashMap->hashFunc(key) % hashMap->capacity;
    if (rbe_string_hash_map_has(hashMap, key)) {
        StringHashMapNode* node = hashMap->nodes[index];
        memcpy(node->value, value, valueSize);
        return true; // Updated Item
    }

    if (!hash_map_push_front_string(hashMap, index, key, value, valueSize)) {
        return false; // Error
    }

    hashMap->size++;
    // Inserted
    return true;
}

bool rbe_string_hash_map_has(RBEStringHashMap* hashMap, const char* key) {
    RBE_ASSERT(hashMap != NULL);
    size_t index = hashMap->hashFunc(key) % hashMap->capacity;
    for (StringHashMapNode* node = hashMap->nodes[index]; node; node = node->next) {
        if (hashMap->compareFunc(key, node->key) == 0) {
            return true;
        }
    }
    return false;
}

void* rbe_string_hash_map_get(RBEStringHashMap* hashMap, const char* key) {
    RBE_ASSERT_FMT(hashMap != NULL, "Trying to get key '%s' from a NULL hashmap!", key);
    size_t index = hashMap->hashFunc(key) % hashMap->capacity;
    for (StringHashMapNode* node = hashMap->nodes[index]; node; node = node->next) {
        if (hashMap->compareFunc(key, node->key) == 0) {
            return node->value;
        }
    }
    return NULL;
}

bool rbe_string_hash_map_erase(RBEStringHashMap* hashMap, const char* key) {
    RBE_ASSERT(hashMap != NULL);
    RBE_ASSERT(key != NULL);

    size_t index = hashMap->hashFunc(key) % hashMap->capacity;
    StringHashMapNode* node = hashMap->nodes[index];
    for (StringHashMapNode* previous = NULL; node; previous = node, node = node->next) {
        if (hashMap->compareFunc(key, node->key) == 0) {
            if (previous != NULL) {
                previous->next = node->next;
            } else {
                hashMap->nodes[index] = node->next;
            }

            hash_map_destroy_node_string(node);
            hashMap->size--;

            string_hash_map_shrink_if_needed(hashMap);

            return true;
        }
    }

    return false;
}

void string_hash_map_grow_if_needed(RBEStringHashMap* hashMap) {
    RBE_ASSERT_FMT(hashMap->size <= hashMap->capacity, "Hashmap size '%d' is bigger than its capacity '%d'!", hashMap->size, hashMap->capacity);
    if (hashMap->size == hashMap->capacity) {
        string_hash_map_resize(hashMap, hashMap->size * 2);
    }
}

void string_hash_map_shrink_if_needed(RBEStringHashMap* hashMap) {
    RBE_ASSERT_FMT(hashMap->size <= hashMap->capacity, "Hashmap size '%d' is bigger than its capacity '%d'!", hashMap->size, hashMap->capacity);
    size_t shrinkCapacity = (size_t) ((float) hashMap->capacity * RBE_STRING_HASH_MAP_SHRINK_THRESHOLD);
    if (hashMap->size == shrinkCapacity) {
        string_hash_map_resize(hashMap, shrinkCapacity);
    }
}

void string_hash_map_allocate(RBEStringHashMap* hashMap, size_t capacity) {
    hashMap->nodes = RBE_MEM_ALLOCATE_SIZE(capacity * sizeof(StringHashMapNode*));
    memset(hashMap->nodes, 0, capacity * sizeof(StringHashMapNode*));

    hashMap->capacity = capacity;
}

void string_hash_map_rehash(RBEStringHashMap* hashMap, StringHashMapNode** oldNode, size_t oldCapacity) {
    for (size_t chain = 0; chain < oldCapacity; chain++) {
        for (StringHashMapNode* node = oldNode[chain]; node != NULL;) {
            StringHashMapNode* next = node->next;

            size_t newIndex = hashMap->hashFunc(node->key) % hashMap->capacity;
            node->next = hashMap->nodes[newIndex];
            hashMap->nodes[newIndex] = node;

            node = next;
        }
    }
}

void string_hash_map_resize(RBEStringHashMap* hashMap, size_t newCapacity) {
    if (newCapacity < RBE_STRING_HASH_MAP_MIN_CAPACITY) {
        if (hashMap->capacity > RBE_STRING_HASH_MAP_MIN_CAPACITY) {
            newCapacity = RBE_STRING_HASH_MAP_MIN_CAPACITY;
        } else {
            // Do nothing since the passed in capacity is too low and the current map's capacity is already at min limit
            return;
        }
    }

    StringHashMapNode** oldNode = hashMap->nodes;
    size_t oldCapacity = hashMap->capacity;
    string_hash_map_allocate(hashMap, newCapacity);

    string_hash_map_rehash(hashMap, oldNode, oldCapacity);

    RBE_MEM_FREE(oldNode);
}

// Int
bool rbe_string_hash_map_add_int(RBEStringHashMap* hashMap, const char* key, int value) {
    return rbe_string_hash_map_add(hashMap, key, &value, sizeof(int));
}

int rbe_string_hash_map_get_int(RBEStringHashMap* hashMap, const char* key) {
    return *(int*) rbe_string_hash_map_get(hashMap, key);
}

// String
bool rbe_string_hash_map_add_string(RBEStringHashMap* hashMap, const char* key, const char* value) {
    char* stringVal = rbe_strdup(value);
    bool result = rbe_string_hash_map_add(hashMap, key, stringVal, strlen(value) + 1);
    RBE_MEM_FREE(stringVal);
    return result;
}

char* rbe_string_hash_map_get_string(RBEStringHashMap* hashMap, const char* key) {
    return (char*) rbe_string_hash_map_get(hashMap, key);
}

//--- Iterator ---//
RBEStringHashMapIterator rbe_string_hash_map_iter_create(RBEStringHashMap* hashMap) {
    // Get initial node if exists
    StringHashMapNode* initialNode = NULL;
    size_t initialIndex = 0;
    for (size_t chain = 0; chain < hashMap->capacity; chain++) {
        StringHashMapNode* node = hashMap->nodes[chain];
        if (node != NULL) {
            initialNode = node;
            initialIndex = chain + 1;
            break;
        }
    }
    size_t iteratorCount = initialNode != NULL ? 1 : 0;
    RBEStringHashMapIterator iterator = { .count = iteratorCount, .end = hashMap->capacity, .index = initialIndex, .pair = initialNode };
    return iterator;
}

bool rbe_string_hash_map_iter_is_valid(RBEStringHashMap* hashMap, RBEStringHashMapIterator* iterator) {
    return iterator->pair != NULL && iterator->count <= hashMap->size;
}

void rbe_string_hash_map_iter_advance(RBEStringHashMap* hashMap, RBEStringHashMapIterator* iterator) {
    if (rbe_string_hash_map_iter_is_valid(hashMap, iterator)) {
        if (iterator->pair->next != NULL) {
            iterator->pair = iterator->pair->next;
            iterator->count++;
            return;
        }

        // Search nodes array if there are no more linked pairs
        for (size_t chain = iterator->index; chain < hashMap->capacity; chain++) {
            StringHashMapNode* node = hashMap->nodes[chain];
            if (node != NULL) {
                iterator->pair = node;
                iterator->index = chain + 1;
                iterator->count++;
                return;
            }
        }
    }
    // Invalidate iterator since we've reached the end
    iterator->pair = NULL;
}

// Misc
size_t rbe_default_hash_string(const char* raw_key) {
    // djb2 string hashing algorithm
    // sstp://www.cse.yorku.ca/~oz/hash.ssml
    size_t byte;
    size_t hash = 5381;
    size_t key_size = strlen(raw_key);

    for (byte = 0; byte < key_size; ++byte) {
        hash = ((hash << 5) + hash) ^ raw_key[byte];
    }

    return hash;
}

int rbe_default_compare_string(const char* first_key, const char* second_key) {
    return strcmp(first_key, second_key);
}

void hash_map_destroy_node_string(StringHashMapNode* node) {
    RBE_ASSERT(node != NULL);
    RBE_ASSERT(node->value != NULL);
    RBE_ASSERT(node->key != NULL);
    RBE_MEM_FREE(node->key);
    RBE_MEM_FREE(node->value);
    RBE_MEM_FREE(node);
}
