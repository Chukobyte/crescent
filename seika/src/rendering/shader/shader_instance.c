#include "shader_instance.h"

#include <string.h>
#include <stdbool.h>

#include "../../memory/se_mem.h"
#include "../../utils/logger.h"
#include "../../utils/se_assert.h"
#include "../../utils/se_string_util.h"

SEShaderInstance* se_shader_instance_create(const char* vertexSource, const char* fragmentSource) {
    SEShader* shader = se_shader_compile_new_shader(vertexSource, fragmentSource);
    // Early out if shader fails to compile
    if (shader == NULL) {
        return NULL;
    }
    SEShaderInstance* instance = SE_MEM_ALLOCATE(SEShaderInstance);
    instance->shader = shader;
    instance->paramMap = se_string_hash_map_create_default_capacity();
    instance->paramsDirty = true;
    return instance;
}

SEShaderInstance* se_shader_instance_create_from_shader(SEShader* shader) {
    SE_ASSERT(shader != NULL);
    SEShaderInstance* instance = SE_MEM_ALLOCATE(SEShaderInstance);
    instance->shader = shader;
    instance->paramMap = se_string_hash_map_create_default_capacity();
    instance->paramsDirty = true;
    return instance;
}

void se_shader_instance_destroy(SEShaderInstance* shaderInstance) {
    SE_STRING_HASH_MAP_FOR_EACH(shaderInstance->paramMap, iter) {
        StringHashMapNode* node = iter.pair;
        SEShaderParam* param = (SEShaderParam*) node->value;
        SE_MEM_FREE(param->name);
    }
    se_string_hash_map_destroy(shaderInstance->paramMap);
    se_shader_destroy(shaderInstance->shader);
    SE_MEM_FREE(shaderInstance);
}

// Creation functions
void se_shader_instance_param_create_from_copy(SEShaderInstance* shaderInstance, SEShaderParam* param) {
    param->name = se_strdup(param->name);
    switch (param->type) {
    case SEShaderParamType_BOOL: {
        se_shader_instance_param_create_bool(shaderInstance, param->name, param->value.boolValue);
        return;
    }
    case SEShaderParamType_INT: {
        se_shader_instance_param_create_int(shaderInstance, param->name, param->value.intValue);
        return;
    }
    case SEShaderParamType_FLOAT: {
        se_shader_instance_param_create_float(shaderInstance, param->name, param->value.floatValue);
        return;
    }
    case SEShaderParamType_FLOAT2: {
        se_shader_instance_param_create_float2(shaderInstance, param->name, param->value.float2Value);
        return;
    }
    case SEShaderParamType_FLOAT3: {
        se_shader_instance_param_create_float3(shaderInstance, param->name, param->value.float3Value);
        return;
    }
    case SEShaderParamType_FLOAT4: {
        se_shader_instance_param_create_float4(shaderInstance, param->name, param->value.float4Value);
        return;
    }
    }
    SE_ASSERT_FMT(false, "Failed to copy shader param with name '%s'", param->name);
}

SEShaderParam* se_shader_instance_param_create_bool(SEShaderInstance* shaderInstance, const char* name, bool value) {
    SEShaderParam params = { .name = se_strdup(name), .type = SEShaderParamType_BOOL };
    params.value.boolValue = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(SEShaderParam));
    return (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

SEShaderParam* se_shader_instance_param_create_int(SEShaderInstance* shaderInstance, const char* name, int value) {
    SEShaderParam params = { .name = se_strdup(name), .type = SEShaderParamType_INT };
    params.value.intValue = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(SEShaderParam));
    return (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

SEShaderParam* se_shader_instance_param_create_float(SEShaderInstance* shaderInstance, const char* name, float value) {
    SEShaderParam params = { .name = se_strdup(name), .type = SEShaderParamType_FLOAT };
    params.value.floatValue = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(SEShaderParam));
    return (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

SEShaderParam* se_shader_instance_param_create_float2(SEShaderInstance* shaderInstance, const char* name, SEVector2 value) {
    SEShaderParam params = { .name = se_strdup(name), .type = SEShaderParamType_FLOAT2 };
    params.value.float2Value = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(SEShaderParam));
    return (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

SEShaderParam* se_shader_instance_param_create_float3(SEShaderInstance* shaderInstance, const char* name, SEVector3 value) {
    SEShaderParam params = { .name = se_strdup(name), .type = SEShaderParamType_FLOAT3 };
    params.value.float3Value = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(SEShaderParam));
    return (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

SEShaderParam* se_shader_instance_param_create_float4(SEShaderInstance* shaderInstance, const char* name, SEVector4 value) {
    SEShaderParam params = { .name = se_strdup(name), .type = SEShaderParamType_FLOAT4 };
    params.value.float4Value = value;
    se_string_hash_map_add(shaderInstance->paramMap, name, &params, sizeof(SEShaderParam));
    return (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
}

// Update functions
void se_shader_instance_param_update_bool(SEShaderInstance* shaderInstance, const char* name, bool value) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_BOOL);
    param->value.boolValue = value;
    shaderInstance->paramsDirty = true;
}

void se_shader_instance_param_update_int(SEShaderInstance* shaderInstance, const char* name, int value) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_INT);
    param->value.intValue = value;
    shaderInstance->paramsDirty = true;
}

void se_shader_instance_param_update_float(SEShaderInstance* shaderInstance, const char* name, float value) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT_FMT(param != NULL, "Shader param for '%s' is null!", name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT);
    param->value.floatValue = value;
    shaderInstance->paramsDirty = true;
}

void se_shader_instance_param_update_float2(SEShaderInstance* shaderInstance, const char* name, SEVector2 value) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT2);
    param->value.float2Value = value;
    shaderInstance->paramsDirty = true;
}

void se_shader_instance_param_update_float3(SEShaderInstance* shaderInstance, const char* name, SEVector3 value) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT3);
    param->value.float3Value = value;
    shaderInstance->paramsDirty = true;
}

void se_shader_instance_param_update_float4(SEShaderInstance* shaderInstance, const char* name, SEVector4 value) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT4);
    param->value.float4Value = value;
    shaderInstance->paramsDirty = true;
}

// Get functions
bool se_shader_instance_param_get_bool(SEShaderInstance* shaderInstance, const char* name) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_BOOL);
    return param->value.boolValue;
}

int se_shader_instance_param_get_int(SEShaderInstance* shaderInstance, const char* name) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_INT);
    return param->value.intValue;
}

float se_shader_instance_param_get_float(SEShaderInstance* shaderInstance, const char* name) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT);
    return param->value.floatValue;
}

SEVector2 se_shader_instance_param_get_float2(SEShaderInstance* shaderInstance, const char* name) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT2);
    return param->value.float2Value;
}

SEVector3 se_shader_instance_param_get_float3(SEShaderInstance* shaderInstance, const char* name) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT3);
    return param->value.float3Value;
}

SEVector4 se_shader_instance_param_get_float4(SEShaderInstance* shaderInstance, const char* name) {
    SEShaderParam* param = (SEShaderParam*) se_string_hash_map_get(shaderInstance->paramMap, name);
    SE_ASSERT(param->type == SEShaderParamType_FLOAT4);
    return param->value.float4Value;
}
