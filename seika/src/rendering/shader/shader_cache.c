#include "shader_cache.h"
#include "../../utils/se_assert.h"

SEStringHashMap* shaderInstanceMap = NULL;

void shader_cache_initialize() {
    SE_ASSERT(shaderInstanceMap == NULL);
    shaderInstanceMap = se_string_hash_map_create_default_capacity();
}

void shader_cache_finalize() {
    SE_ASSERT(shaderInstanceMap != NULL);
    se_string_hash_map_destroy(shaderInstanceMap);
    shaderInstanceMap = NULL;
}

void shader_cache_add_instance(ShaderInstance* instance, const char* id) {
    SE_ASSERT(shaderInstanceMap != NULL);
    se_string_hash_map_add(shaderInstanceMap, id, &instance, sizeof(ShaderInstance**));
}

void shader_cache_remove_instance(const char* id) {
    SE_ASSERT(shaderInstanceMap != NULL);
    se_string_hash_map_erase(shaderInstanceMap, id);
}

ShaderInstance* shader_cache_get_instance(const char* id) {
    ShaderInstance* instance = (ShaderInstance*) *(ShaderInstance**) se_string_hash_map_get(shaderInstanceMap, id);
    return instance;
}
