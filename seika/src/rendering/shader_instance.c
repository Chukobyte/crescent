#include "shader_instance.h"

#include <string.h>
#include <stdbool.h>

#include "../memory/se_mem.h"
#include "../utils/logger.h"
#include "../utils/se_assert.h"
#include "../utils/se_string_util.h"
#include "../math/se_math.h"

size_t get_shader_param_type_size(ShaderParamType type);

ShaderInstance* se_shader_instance_create(const char* vertexSource, const char* fragmentSource) {
    Shader* shader = shader_compile_new_shader(vertexSource, fragmentSource);
    // Early out if shader fails to compile
    if (shader == NULL) {
        return NULL;
    }
    ShaderInstance* instance = SE_MEM_ALLOCATE(ShaderInstance);
    instance->shader = shader;
    instance->paramMap = se_string_hash_map_create_default_capacity();
    return instance;
}

void se_shader_instance_destroy(ShaderInstance* shaderInstance) {
    SE_STRING_HASH_MAP_FOR_EACH(shaderInstance->paramMap, iter) {
        StringHashMapNode* node = iter.pair;
        ShaderParam* param = (ShaderParam*) *(ShaderParam**) node->value;
        SE_MEM_FREE(param->name);
        SE_MEM_FREE(param->value);
        SE_MEM_FREE(param);
    }
    se_string_hash_map_destroy(shaderInstance->paramMap);
    shader_destroy(shaderInstance->shader);
    SE_MEM_FREE(shaderInstance);
}

ShaderParam* se_shader_instance_create_param(ShaderInstance* shaderInstance, const char* name, void* value, ShaderParamType valueType) {
    SE_ASSERT_FMT(!se_string_hash_map_has(shaderInstance->paramMap, name), "Shader param with name '%s' already exists!", name);
    ShaderParam* param = SE_MEM_ALLOCATE(ShaderParam);
    param->name = se_strdup(name);
    param->type = valueType;
    param->valueSize = get_shader_param_type_size(param->type);
    param->value = SE_MEM_ALLOCATE_SIZE(param->valueSize);
    memcpy(param->value, value, param->valueSize);
    return param;
}

void se_shader_instance_set_param_value(ShaderInstance* shaderInstance, const char* name, void* value) {
    if (!se_string_hash_map_has(shaderInstance->paramMap, name)) {
        se_logger_warn("Trying to set shader param for '%s' but it doesn't exist!", name);
        return;
    }
    ShaderParam* param = (ShaderParam*) *(ShaderParam**) se_string_hash_map_get(shaderInstance->paramMap, name);
    memcpy(param->value, value, param->valueSize);
}

void* se_shader_instance_get_param_value(ShaderInstance* shaderInstance, const char* name) {
    if (!se_string_hash_map_has(shaderInstance->paramMap, name)) {
        se_logger_warn("Trying to set shader param for '%s' but it doesn't exist!", name);
        return NULL;
    }
    ShaderParam* param = (ShaderParam*) *(ShaderParam**) se_string_hash_map_get(shaderInstance->paramMap, name);
    return param->value;
}

size_t get_shader_param_type_size(ShaderParamType type) {
    switch (type) {
    case ShaderParamType_BOOL:
        return sizeof(bool);
    case ShaderParamType_INT:
        return sizeof(int);
    case ShaderParamType_FLOAT:
        return sizeof(float);
    case ShaderParamType_FLOAT2:
        return sizeof(Vector2);
    case ShaderParamType_FLOAT3:
        return sizeof(Vector3);
    case ShaderParamType_FLOAT4:
        return sizeof(Vector4);
    default:
        return 0;
    }
}
