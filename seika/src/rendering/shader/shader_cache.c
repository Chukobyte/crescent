#include "shader_cache.h"

#include <string.h>

#include "shader_file_parser.h"
#include "../../data_structures/se_queue.h"
#include "../../asset/asset_file_loader.h"
#include "../../utils/se_assert.h"
#include "../../memory/se_mem.h"

static SEShaderInstance* instanceCache[SE_SHADER_INSTANCE_MAX_INSTANCES];
static SEQueue* shaderInstanceIdQueue = NULL;

void se_shader_cache_initialize() {
    SE_ASSERT(shaderInstanceIdQueue == NULL);
    shaderInstanceIdQueue = se_queue_create(SE_SHADER_INSTANCE_MAX_INSTANCES, SE_SHADER_INSTANCE_INVALID_ID);
    for (size_t i = 0; i < SE_SHADER_INSTANCE_MAX_INSTANCES; i++) {
        se_queue_enqueue(shaderInstanceIdQueue, i);
        instanceCache[i] = NULL;
    }
}

void se_shader_cache_finalize() {
    SE_ASSERT(shaderInstanceIdQueue != NULL);
    se_queue_destroy(shaderInstanceIdQueue);
    shaderInstanceIdQueue = NULL;
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
    SEShaderInstanceId newId = SE_SHADER_INSTANCE_INVALID_ID;
    char* shaderSource = sf_asset_file_loader_read_file_contents_as_string_without_raw(shaderPath, NULL);
    // Uncomment when needing to debug shaders
//    se_logger_debug("shader source = \n%s", shaderSource);
    if (shaderSource) {
        SEShaderFileParseResult result = se_shader_file_parser_parse_shader(shaderSource);
        const bool hasErrorMessage = strlen(result.errorMessage) > 0;
        if (!hasErrorMessage) {
            newId = se_shader_cache_create_instance_and_add_from_source(result.parseData.fullVertexSource, result.parseData.fullFragmentSource);
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(newId);
            if (shaderInstance) {
                for (size_t i = 0; i < result.parseData.uniformCount; i++) {
                    se_shader_instance_param_create(shaderInstance, result.parseData.uniforms[i]);
                }
            }
            se_shader_file_parse_clear_parse_result(&result);
        } else {
            se_logger_error("Shader parse error = '%s'\n", result.errorMessage);
        }
    }
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
