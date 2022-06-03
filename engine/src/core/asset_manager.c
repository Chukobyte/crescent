#include "asset_manager.h"

#include "data_structures/rbe_hash_map_string.h"
#include "rendering/texture.h"
#include "utils/rbe_assert.h"

#define INITIAL_ASSET_HASHMAP_SIZE 64

RBEStringHashMap* texturesMap = NULL;

void rbe_asset_manager_initialize() {
    texturesMap = rbe_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
}

Texture* rbe_asset_manager_load_texture(const char* fileName, const char* key) {
    RBE_ASSERT(texturesMap != NULL);
    RBE_ASSERT_FMT(!rbe_string_hash_map_has(texturesMap, fileName), "Already loaded texture at file path '%'s!  Has key '%s'.", fileName, key);
    Texture* texture = rbe_texture_create_texture(fileName);
    rbe_string_hash_map_add(texturesMap, key, texture, sizeof(Texture));
    return texture;
}

Texture* rbe_asset_manager_get_texture(const char* key) {
    return rbe_string_hash_map_get(texturesMap, key);
}
