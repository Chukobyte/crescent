#pragma once

/*
 * Shader Instance
 * ---------------------------------------------------------------------------------------------------------------------
 * A work in progress api to create shader instances with parameters.
 */

#include "shader.h"

#include "../../data_structures/se_hash_map_string.h"

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
    ShaderParamType type;
    union {
        bool boolValue;
        int intValue;
        float floatValue;
        Vector2 float2Value;
        Vector3 float3Value;
        Vector4 float4Value;
    } value;
} ShaderParam;

typedef struct ShaderInstance {
    Shader* shader;
    SEStringHashMap* paramMap;
} ShaderInstance;

ShaderInstance* se_shader_instance_create(const char* vertexSource, const char* fragmentSource);
void se_shader_instance_destroy(ShaderInstance* shaderInstance);

ShaderParam* se_shader_instance_param_create_bool(ShaderInstance* shaderInstance, const char* name, bool value);
ShaderParam* se_shader_instance_param_create_int(ShaderInstance* shaderInstance, const char* name, int value);
ShaderParam* se_shader_instance_param_create_float(ShaderInstance* shaderInstance, const char* name, float value);
ShaderParam* se_shader_instance_param_create_float2(ShaderInstance* shaderInstance, const char* name, Vector2 value);
ShaderParam* se_shader_instance_param_create_float3(ShaderInstance* shaderInstance, const char* name, Vector3 value);
ShaderParam* se_shader_instance_param_create_float4(ShaderInstance* shaderInstance, const char* name, Vector4 value);

void se_shader_instance_param_update_bool(ShaderInstance* shaderInstance, const char* name, bool value);
void se_shader_instance_param_update_int(ShaderInstance* shaderInstance, const char* name, int value);
void se_shader_instance_param_update_float(ShaderInstance* shaderInstance, const char* name, float value);
void se_shader_instance_param_update_float2(ShaderInstance* shaderInstance, const char* name, Vector2 value);
void se_shader_instance_param_update_float3(ShaderInstance* shaderInstance, const char* name, Vector3 value);
void se_shader_instance_param_update_float4(ShaderInstance* shaderInstance, const char* name, Vector4 value);

bool se_shader_instance_param_get_bool(ShaderInstance* shaderInstance, const char* name);
int se_shader_instance_param_get_int(ShaderInstance* shaderInstance, const char* name);
float se_shader_instance_param_get_float(ShaderInstance* shaderInstance, const char* name);
Vector2 se_shader_instance_param_get_float2(ShaderInstance* shaderInstance, const char* name);
Vector3 se_shader_instance_param_get_float3(ShaderInstance* shaderInstance, const char* name);
Vector4 se_shader_instance_param_get_float4(ShaderInstance* shaderInstance, const char* name);
