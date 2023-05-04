#include "shader_cache.h"

#include <string.h>

#include "shader_file_parser.h"
#include "../../data_structures/se_queue.h"
#include "../../asset/asset_file_loader.h"
#include "../../utils/se_assert.h"
#include "../../memory/se_mem.h"

// --- Parsed Shader Cache --- //
typedef struct SEParsedShaderCacheItem {
    SEShaderFileParseResult parseResult;
} SEParsedShaderCacheItem;

SEStringHashMap* parsedShaderCacheMap = NULL;

// --- Shader Cache --- //
static SEShaderInstance* instanceCache[SE_SHADER_INSTANCE_MAX_INSTANCES];
static SEQueue* shaderInstanceIdQueue = NULL;

void se_shader_cache_initialize() {
    SE_ASSERT(shaderInstanceIdQueue == NULL);
    SE_ASSERT(parsedShaderCacheMap == NULL);
    shaderInstanceIdQueue = se_queue_create(SE_SHADER_INSTANCE_MAX_INSTANCES, SE_SHADER_INSTANCE_INVALID_ID);
    for (uint32_t i = 0; i < SE_SHADER_INSTANCE_MAX_INSTANCES; i++) {
        se_queue_enqueue(shaderInstanceIdQueue, i);
        instanceCache[i] = NULL;
    }
    parsedShaderCacheMap = se_string_hash_map_create_default_capacity();
}

void se_shader_cache_finalize() {
    SE_ASSERT(shaderInstanceIdQueue != NULL);
    SE_ASSERT(parsedShaderCacheMap != NULL);
    se_queue_destroy(shaderInstanceIdQueue);
    shaderInstanceIdQueue = NULL;
    SE_STRING_HASH_MAP_FOR_EACH(parsedShaderCacheMap, iter) {
        StringHashMapNode *node = iter.pair;
        SEParsedShaderCacheItem* cacheItem = (SEParsedShaderCacheItem*) node->value;
        se_shader_file_parse_clear_parse_result(&cacheItem->parseResult);
    }
    se_string_hash_map_destroy(parsedShaderCacheMap);
    parsedShaderCacheMap = NULL;
}

SEShaderInstanceId se_shader_cache_add_instance(SEShaderInstance* instance) {
    const SEShaderInstanceId newId = se_queue_dequeue(shaderInstanceIdQueue);
    instanceCache[newId] = instance;
    return newId;
}

void se_shader_cache_remove_instance(SEShaderInstanceId instanceId) {
    instanceCache[instanceId] = NULL;
    se_queue_enqueue(shaderInstanceIdQueue, instanceId);
}

SEShaderInstance* se_shader_cache_get_instance(SEShaderInstanceId instanceId) {
    return instanceCache[instanceId];
}

SEShaderInstance* se_shader_cache_get_instance_checked(SEShaderInstanceId instanceId) {
    if (instanceId != SE_SHADER_INSTANCE_INVALID_ID) {
        return instanceCache[instanceId];
    }
    return NULL;
}

SEShaderInstanceId se_shader_cache_create_instance_and_add(const char* shaderPath) {
    if (!se_string_hash_map_has(parsedShaderCacheMap, shaderPath)) {
        char* shaderSource = sf_asset_file_loader_read_file_contents_as_string_without_raw(shaderPath, NULL);
        // Uncomment when needing to debug shaders
//    se_logger_debug("shader source = \n%s", shaderSource);
        if (shaderSource) {
            SEParsedShaderCacheItem newCacheItem;
            newCacheItem.parseResult = se_shader_file_parser_parse_shader(shaderSource);
            const bool hasErrorMessage = strlen(newCacheItem.parseResult.errorMessage) > 0;
            if (hasErrorMessage) {
                se_logger_error("Shader parse error = '%s'\n", newCacheItem.parseResult.errorMessage);
                return SE_SHADER_INSTANCE_INVALID_ID;
            }
            se_string_hash_map_add(parsedShaderCacheMap, shaderPath, &newCacheItem, sizeof(SEParsedShaderCacheItem));
        } else {
            se_logger_error("Failed to read shader source from '%s'", shaderPath);
            return SE_SHADER_INSTANCE_INVALID_ID;
        }
    }

    SEParsedShaderCacheItem* cacheItem = (SEParsedShaderCacheItem*) se_string_hash_map_get(parsedShaderCacheMap, shaderPath);
    SEShader* newShader = se_shader_compile_new_shader(cacheItem->parseResult.parseData.fullVertexSource, cacheItem->parseResult.parseData.fullFragmentSource);
    if (newShader == NULL) {
        se_logger_error("Error compiling shader from path = '%s'\n", shaderPath);
        return SE_SHADER_INSTANCE_INVALID_ID;
    }
    SEShaderInstance* shaderInstance = se_shader_instance_create_from_shader(newShader);
    for (size_t i = 0; i < cacheItem->parseResult.parseData.uniformCount; i++) {
        se_shader_instance_param_create_from_copy(shaderInstance, &cacheItem->parseResult.parseData.uniforms[i]);
    }
    SEShaderInstanceId newId = se_shader_cache_add_instance(shaderInstance);
    return newId;
}

SEShaderInstanceId se_shader_cache_create_instance_and_add_from_raw(const char* vertexPath, const char* fragmentPath) {
    char* vertexSource = sf_asset_file_loader_read_file_contents_as_string(vertexPath, NULL);
    char* fragmentSource = sf_asset_file_loader_read_file_contents_as_string(fragmentPath, NULL);
    const SEShaderInstanceId newId = se_shader_cache_create_instance_and_add_from_source(vertexSource, fragmentSource);
    SE_MEM_FREE(vertexSource);
    SE_MEM_FREE(fragmentSource);
    return newId;
}

SEShaderInstanceId se_shader_cache_create_instance_and_add_from_source(const char* vertexSource, const char* fragmentSource) {
    SEShaderInstance* instance = se_shader_instance_create(vertexSource, fragmentSource);
    const SEShaderInstanceId newId = se_shader_cache_add_instance(instance);
    return newId;
}
