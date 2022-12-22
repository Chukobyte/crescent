#pragma once

#include "../data_structures/se_hash_map_string.h"

typedef enum ShaderParamType {
    ShaderParamType_BOOL = 0,
    ShaderParamType_INT = 1,
    ShaderParamType_FLOAT = 2,
    ShaderParamType_FLOAT2 = 3,
    ShaderParamType_FLOAT3 = 4,
    ShaderParamType_FLOAT4 = 5,
} ShaderParamType;

typedef struct ShaderParam {
    char* name;
    void* value;
    ShaderParamType type;
} ShaderParam;

typedef struct ShaderInstance {
    SEStringHashMap* paramMap;
} ShaderInstance;

ShaderInstance* se_shader_instance_create(const char* shaderDefinitionPath);
void se_shader_instance_destroy(ShaderInstance* shaderInstance);
ShaderParam* se_shader_instance_create_param(ShaderInstance* shaderInstance, const char* name, void* value, ShaderParamType valueType);
void se_shader_instance_set_param_value(ShaderInstance* shaderInstance, const char* name, void* value);
void* se_shader_instance_get_param_value(ShaderInstance* shaderInstance, const char* name);
