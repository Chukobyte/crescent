#include "se_hash_map.h"

#include <string.h>

#include "../memory/se_mem.h"
#include "../utils/se_assert.h"

size_t se_default_hash(void* raw_key, size_t key_size);
int se_default_compare(void* first_key, void* second_key, size_t key_size);

HashMapNode* hash_map_create_node(SEHashMap* hashMap, void* key, void* value, HashMapNode* next);
void hash_map_destroy_node(HashMapNode* node);

bool hash_map_push_front(SEHashMap* hashMap, size_t index, void* key, void* value);
void hash_map_grow_if_needed(SEHashMap* hashMap);
void hash_map_shrink_if_needed(SEHashMap* hashMap);
void hash_map_allocate(SEHashMap* hashMap, size_t capacity);
void hash_map_resize(SEHashMap* hashMap, size_t newCapacity);

SEHashMap* se_hash_map_create(size_t keySize, size_t valueSize, size_t capacity) {
    SEHashMap* map = (SEHashMap*) SE_MEM_ALLOCATE_SIZE(sizeof(SEHashMap));
    map->keySize = keySize;
    map->valueSize = valueSize;
    map->capacity = capacity;
    map->size = 0;
    map->hashFunc = se_default_hash;
    map->compareFunc = se_default_compare;
    map->nodes = (HashMapNode**) SE_MEM_ALLOCATE_SIZE(capacity * sizeof(HashMapNode*));
    memset(map->nodes, 0, capacity * sizeof(HashMapNode*)); // TODO: fix
    return map;
}

HashMapNode* hash_map_create_node(SEHashMap* hashMap, void* key, void* value, HashMapNode* next) {
    HashMapNode* node = (HashMapNode*) SE_MEM_ALLOCATE(HashMapNode);
    node->key = SE_MEM_ALLOCATE_SIZE(hashMap->keySize);
    node->value = SE_MEM_ALLOCATE_SIZE(hashMap->valueSize);
    memcpy(node->key, key, hashMap->keySize);
    memcpy(node->value, value, hashMap->valueSize);
    node->next = next;
    return node;
}

bool se_hash_map_destroy(SEHashMap* hashMap) {
    HashMapNode* node = NULL;
    HashMapNode* next = NULL;
    for (size_t chain = 0; chain < hashMap->capacity; chain++) {
        node = hashMap->nodes[chain];
        while (node != NULL) {
            next = node->next;
            hash_map_destroy_node(node);
            node = next;
        }
    }

    SE_MEM_FREE(hashMap);

    return true;
}

bool hash_map_push_front(SEHashMap* hashMap, size_t index, void* key, void* value) {
    hashMap->nodes[index] = hash_map_create_node(hashMap, key, value, hashMap->nodes[index]);
    return hashMap->nodes[index] != NULL;
}

bool se_hash_map_add(SEHashMap* hashMap, void* key, void* value) {
    SE_ASSERT(hashMap != NULL);
    SE_ASSERT(key != NULL);
    SE_ASSERT(value != NULL);

    hash_map_grow_if_needed(hashMap);

    size_t index = hashMap->hashFunc(key, hashMap->keySize) % hashMap->capacity;
    if (se_hash_map_has(hashMap, key)) {
        HashMapNode* node = hashMap->nodes[index];
        memcpy(node->value, value, hashMap->valueSize);
        return true; // Updated Item
    }

    if (!hash_map_push_front(hashMap, index, key, value)) {
        return false; // Error
    }

    hashMap->size++;
    // Inserted
    return true;
}

bool se_hash_map_has(SEHashMap* hashMap, void* key) {
    SE_ASSERT(hashMap != NULL);
    size_t index = hashMap->hashFunc(key, hashMap->keySize) % hashMap->capacity;
    for (HashMapNode* node = hashMap->nodes[index]; node; node = node->next) {
        if (hashMap->compareFunc(key, node->key, hashMap->keySize) == 0) {
            return true;
        }
    }
    return false;
}

void* se_hash_map_get(SEHashMap* hashMap, void* key) {
    SE_ASSERT(hashMap != NULL);
    size_t index = hashMap->hashFunc(key, hashMap->keySize) % hashMap->capacity;
    for (HashMapNode* node = hashMap->nodes[index]; node; node = node->next) {
        if (hashMap->compareFunc(key, node->key, hashMap->keySize) == 0) {
            return node->value;
        }
    }
    return NULL;
}

bool se_hash_map_erase(SEHashMap* hashMap, void* key) {
    SE_ASSERT(hashMap != NULL);
    SE_ASSERT(key != NULL);

    size_t index = hashMap->hashFunc(key, hashMap->keySize) % hashMap->capacity;
    HashMapNode* node = hashMap->nodes[index];
    for (HashMapNode* previous = NULL; node; previous = node, node = node->next) {
        if (hashMap->compareFunc(key, node->key, hashMap->keySize) == 0) {
            if (previous != NULL) {
                previous->next = node->next;
            } else {
                hashMap->nodes[index] = node->next;
            }

            hash_map_destroy_node(node);
            hashMap->size--;

            hash_map_shrink_if_needed(hashMap);

            return true;
        }
    }

    return false;
}

void hash_map_grow_if_needed(SEHashMap* hashMap) {
    SE_ASSERT_FMT(hashMap->size <= hashMap->capacity, "Hashmap size '%d' is bigger than its capacity '%d'!", hashMap->size, hashMap->capacity);
    if (hashMap->size == hashMap->capacity) {
        hash_map_resize(hashMap, hashMap->size * 2);
    }
}

void hash_map_shrink_if_needed(SEHashMap* hashMap) {
    SE_ASSERT_FMT(hashMap->size <= hashMap->capacity, "Hashmap size '%d' is bigger than its capacity '%d'!", hashMap->size, hashMap->capacity);
    size_t shrinkCapacity = (size_t) ((float) hashMap->capacity * SE_HASH_MAP_SHRINK_THRESHOLD);
    if (hashMap->size == shrinkCapacity) {
        hash_map_resize(hashMap, shrinkCapacity);
    }
}

void hash_map_allocate(SEHashMap* hashMap, size_t capacity) {
    hashMap->nodes = SE_MEM_ALLOCATE_SIZE(capacity * sizeof(HashMapNode*));
    memset(hashMap->nodes, 0, capacity * sizeof(HashMapNode*));

    hashMap->capacity = capacity;
}

void hash_map_rehash(SEHashMap* hashMap, HashMapNode** oldNode, size_t oldCapacity) {
    for (size_t chain = 0; chain < oldCapacity; chain++) {
        for (HashMapNode* node = oldNode[chain]; node != NULL;) {
            HashMapNode* next = node->next;

            size_t newIndex = hashMap->hashFunc(node->key, hashMap->keySize) % hashMap->capacity;
            node->next = hashMap->nodes[newIndex];
            hashMap->nodes[newIndex] = node;

            node = next;
        }
    }
}

void hash_map_resize(SEHashMap* hashMap, size_t newCapacity) {
    if (newCapacity < SE_HASH_MAP_MIN_CAPACITY) {
        if (hashMap->capacity > SE_HASH_MAP_MIN_CAPACITY) {
            newCapacity = SE_HASH_MAP_MIN_CAPACITY;
        } else {
            // Do nothing since the passed in capacity is too low and the current map's capacity is already at min limit
            return;
        }
    }

    HashMapNode** oldNode = hashMap->nodes;
    size_t oldCapacity = hashMap->capacity;
    hash_map_allocate(hashMap, newCapacity);

    hash_map_rehash(hashMap, oldNode, oldCapacity);

    SE_MEM_FREE(oldNode);
}

// --- Iterator --- //
SEHashMapIterator se_hash_map_iter_create(SEHashMap* hashMap) {
    // Get initial node if exists
    HashMapNode* initialNode = NULL;
    size_t initialIndex = 0;
    for (size_t chain = 0; chain < hashMap->capacity; chain++) {
        HashMapNode* node = hashMap->nodes[chain];
        if (node != NULL) {
            initialNode = node;
            initialIndex = chain + 1;
            break;
        }
    }
    size_t iteratorCount = initialNode != NULL ? 1 : 0;
    SEHashMapIterator iterator = { .count = iteratorCount, .end = hashMap->capacity, .index = initialIndex, .pair = initialNode };
    return iterator;
}

bool se_hash_map_iter_is_valid(SEHashMap* hashMap, SEHashMapIterator* iterator) {
    return iterator->pair != NULL && iterator->count <= hashMap->size;
}

void se_hash_map_iter_advance(SEHashMap* hashMap, SEHashMapIterator* iterator) {
    if (se_hash_map_iter_is_valid(hashMap, iterator)) {
        if (iterator->pair->next != NULL) {
            iterator->pair = iterator->pair->next;
            iterator->count++;
            return;
        }

        // Search nodes array if there are no more linked pairs
        for (size_t chain = iterator->index; chain < hashMap->capacity; chain++) {
            HashMapNode* node = hashMap->nodes[chain];
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
size_t se_default_hash(void* raw_key, size_t key_size) {
    // djb2 string hashing algorithm
    // sstp://www.cse.yorku.ca/~oz/hash.ssml
    size_t byte;
    size_t hash = 5381;
    char* key = (char*) raw_key;

    for (byte = 0; byte < key_size; ++byte) {
        // (hash << 5) + hash = hash * 33
        hash = ((hash << 5) + hash) ^ key[byte];
    }

    return hash;
}

int se_default_compare(void* first_key, void* second_key, size_t key_size) {
    return memcmp(first_key, second_key, key_size);
}

void hash_map_destroy_node(HashMapNode* node) {
    SE_ASSERT(node != NULL);
    SE_ASSERT(node->value != NULL);
    SE_ASSERT(node->key != NULL);

    SE_MEM_FREE(node->key);
    node->key = NULL;
    SE_MEM_FREE(node->value);
    node->value = NULL;
    SE_MEM_FREE(node);
}
