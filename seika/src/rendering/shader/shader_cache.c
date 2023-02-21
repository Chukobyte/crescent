#include "shader_cache.h"

#include "../../data_structures/se_queue.h"
#include "../../utils/se_assert.h"

static ShaderInstance* instanceCache[SHADER_CACHE_MAX_INSTANCES];
static SEQueue* shaderInstanceIdQueue = NULL;

void shader_cache_initialize() {
    SE_ASSERT(shaderInstanceIdQueue == NULL);
    shaderInstanceIdQueue = se_queue_create(SHADER_CACHE_MAX_INSTANCES, SHADER_CACHE_INVALID_ID);
    for (size_t i = 0; i < SHADER_CACHE_MAX_INSTANCES; i++) {
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
