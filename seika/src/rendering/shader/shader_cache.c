#include "shader_cache.h"

#include "../../data_structures/se_queue.h"
#include "../../asset/asset_file_loader.h"
#include "../../utils/se_assert.h"
#include "../../memory/se_mem.h"

static ShaderInstance* instanceCache[SHADER_INSTANCE_MAX_INSTANCES];
static SEQueue* shaderInstanceIdQueue = NULL;

void shader_cache_initialize() {
    SE_ASSERT(shaderInstanceIdQueue == NULL);
    shaderInstanceIdQueue = se_queue_create(SHADER_INSTANCE_MAX_INSTANCES, SHADER_INSTANCE_INVALID_ID);
    for (size_t i = 0; i < SHADER_INSTANCE_MAX_INSTANCES; i++) {
        se_queue_enqueue(shaderInstanceIdQueue, i);
        instanceCache[i] = NULL;
    }
}

void shader_cache_finalize() {
    SE_ASSERT(shaderInstanceIdQueue != NULL);
    se_queue_destroy(shaderInstanceIdQueue);
    shaderInstanceIdQueue = NULL;
}

ShaderInstanceId shader_cache_add_instance(ShaderInstance* instance) {
    const ShaderInstanceId newId = se_queue_dequeue(shaderInstanceIdQueue);
    instanceCache[newId] = instance;
    return newId;
}

void shader_cache_remove_instance(ShaderInstanceId instanceId) {
    instanceCache[instanceId] = NULL;
    se_queue_enqueue(shaderInstanceIdQueue, instanceId);
}

ShaderInstance* shader_cache_get_instance(ShaderInstanceId instanceId) {
    return instanceCache[instanceId];
}

ShaderInstance* shader_cache_get_instance_checked(ShaderInstanceId instanceId) {
    if (instanceId != SHADER_INSTANCE_INVALID_ID) {
        return instanceCache[instanceId];
    }
    return NULL;
}

ShaderInstanceId shader_cache_create_instance_and_add(const char* vertexPath, const char* fragmentPath) {
    char* vertexSource = sf_asset_file_loader_read_file_contents_as_string(vertexPath, NULL);
    char* fragmentSource = sf_asset_file_loader_read_file_contents_as_string(fragmentPath, NULL);
    ShaderInstance* instance = se_shader_instance_create(vertexSource, fragmentSource);
    const ShaderInstanceId newId = shader_cache_add_instance(instance);
    SE_MEM_FREE(vertexSource);
    SE_MEM_FREE(fragmentSource);
    return newId;
}
