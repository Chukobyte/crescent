#include "shader_instance.h"

#include <string.h>
#include <stdbool.h>

#include "../memory/se_mem.h"
#include "../utils/logger.h"
#include "../utils/se_assert.h"
#include "../utils/se_string_util.h"
#include "../math/se_math.h"

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
        ShaderParam* param = (ShaderParam*) node->value;
        SE_MEM_FREE(param->name);
    }
    se_string_hash_map_destroy(shaderInstance->paramMap);
    shader_destroy(shaderInstance->shader);
    SE_MEM_FREE(shaderInstance);
}

// Creation functions
ShaderParam* se_shader_instance_param_create_bool(ShaderInstance* shaderInstance, const char* name, bool value) {
    ShaderParam params = { .name = se_strdup(name), .type = ShaderParamType_BOOL };
    params.value.boolValue = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(ShaderParam));
    return (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

ShaderParam* se_shader_instance_param_create_int(ShaderInstance* shaderInstance, const char* name, int value) {
    ShaderParam params = { .name = se_strdup(name), .type = ShaderParamType_INT };
    params.value.intValue = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(ShaderParam));
    return (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

ShaderParam* se_shader_instance_param_create_float(ShaderInstance* shaderInstance, const char* name, float value) {
    ShaderParam params = { .name = se_strdup(name), .type = ShaderParamType_FLOAT };
    params.value.floatValue = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(ShaderParam));
    return (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

ShaderParam* se_shader_instance_param_create_float2(ShaderInstance* shaderInstance, const char* name, Vector2 value) {
    ShaderParam params = { .name = se_strdup(name), .type = ShaderParamType_FLOAT2 };
    params.value.float2Value = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(ShaderParam));
    return (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

ShaderParam* se_shader_instance_param_create_float3(ShaderInstance* shaderInstance, const char* name, Vector3 value) {
    ShaderParam params = { .name = se_strdup(name), .type = ShaderParamType_FLOAT3 };
    params.value.float3Value = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(ShaderParam));
    return (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

ShaderParam* se_shader_instance_param_create_float4(ShaderInstance* shaderInstance, const char* name, Vector4 value) {
    ShaderParam params = { .name = se_strdup(name), .type = ShaderParamType_FLOAT4 };
    params.value.float4Value = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(ShaderParam));
    return (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

// Update functions
void se_shader_instance_param_update_bool(ShaderInstance* shaderInstance, const char* name, bool value) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_BOOL);
    param->value.boolValue = value;
}

void se_shader_instance_param_update_int(ShaderInstance* shaderInstance, const char* name, int value) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_INT);
    param->value.intValue = value;
}

void se_shader_instance_param_update_float(ShaderInstance* shaderInstance, const char* name, float value) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT);
    param->value.floatValue = value;
}

void se_shader_instance_param_update_float2(ShaderInstance* shaderInstance, const char* name, Vector2 value) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT2);
    param->value.float2Value = value;
}

void se_shader_instance_param_update_float3(ShaderInstance* shaderInstance, const char* name, Vector3 value) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT3);
    param->value.float3Value = value;
}

void se_shader_instance_param_update_float4(ShaderInstance* shaderInstance, const char* name, Vector4 value) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT4);
    param->value.float4Value = value;
}

// Get functions
bool se_shader_instance_param_get_bool(ShaderInstance* shaderInstance, const char* name) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_BOOL);
    return param->value.boolValue;
}

int se_shader_instance_param_get_int(ShaderInstance* shaderInstance, const char* name) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_INT);
    return param->value.intValue;
}

float se_shader_instance_param_get_float(ShaderInstance* shaderInstance, const char* name) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT);
    return param->value.floatValue;
}

Vector2 se_shader_instance_param_get_float2(ShaderInstance* shaderInstance, const char* name) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT2);
    return param->value.float2Value;
}

Vector3 se_shader_instance_param_get_float3(ShaderInstance* shaderInstance, const char* name) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT3);
    return param->value.float3Value;
}

Vector4 se_shader_instance_param_get_float4(ShaderInstance* shaderInstance, const char* name) {
    ShaderParam* param = (ShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == ShaderParamType_FLOAT4);
    return param->value.float4Value;
}
