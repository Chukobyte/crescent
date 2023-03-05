#pragma once

/*
 * Shader Instance
 * ---------------------------------------------------------------------------------------------------------------------
 * A work in progress api to create shader instances with parameters.
 */

#include "shader.h"

#include "../../data_structures/se_hash_map_string.h"

// Shader Param - Shader instance representation of uniform parameters
typedef enum SEShaderParamType {
    SEShaderParamType_BOOL = 0,
    SEShaderParamType_INT = 1,
    SEShaderParamType_FLOAT = 2,
    SEShaderParamType_FLOAT2 = 3,
    SEShaderParamType_FLOAT3 = 4,
    SEShaderParamType_FLOAT4 = 5,
} SEShaderParamType;

typedef struct SEShaderParam {
    char* name;
    SEShaderParamType type;
    union {
        bool boolValue;
        int intValue;
        float floatValue;
        SEVector2 float2Value;
        SEVector3 float3Value;
        SEVector4 float4Value;
    } value;
} SEShaderParam;

// Shader Instance
typedef enum SEShaderInstanceType {
    SEShaderInstanceType_INVALID = -1,
    SEShaderInstanceType_SCREEN = 0,
    SEShaderInstanceType_SPRITE = 1,
} SEShaderInstanceType;

typedef struct SEShaderInstance {
    SEShader* shader;
    SEStringHashMap* paramMap;
} SEShaderInstance;

SEShaderInstance* se_shader_instance_create(const char* vertexSource, const char* fragmentSource);
void se_shader_instance_destroy(SEShaderInstance* shaderInstance);

void se_shader_instance_param_create(SEShaderInstance* shaderInstance, SEShaderParam param);
SEShaderParam* se_shader_instance_param_create_bool(SEShaderInstance* shaderInstance, const char* name, bool value);
SEShaderParam* se_shader_instance_param_create_int(SEShaderInstance* shaderInstance, const char* name, int value);
SEShaderParam* se_shader_instance_param_create_float(SEShaderInstance* shaderInstance, const char* name, float value);
SEShaderParam* se_shader_instance_param_create_float2(SEShaderInstance* shaderInstance, const char* name, SEVector2 value);
SEShaderParam* se_shader_instance_param_create_float3(SEShaderInstance* shaderInstance, const char* name, SEVector3 value);
SEShaderParam* se_shader_instance_param_create_float4(SEShaderInstance* shaderInstance, const char* name, SEVector4 value);

void se_shader_instance_param_update_bool(SEShaderInstance* shaderInstance, const char* name, bool value);
void se_shader_instance_param_update_int(SEShaderInstance* shaderInstance, const char* name, int value);
void se_shader_instance_param_update_float(SEShaderInstance* shaderInstance, const char* name, float value);
void se_shader_instance_param_update_float2(SEShaderInstance* shaderInstance, const char* name, SEVector2 value);
void se_shader_instance_param_update_float3(SEShaderInstance* shaderInstance, const char* name, SEVector3 value);
void se_shader_instance_param_update_float4(SEShaderInstance* shaderInstance, const char* name, SEVector4 value);

bool se_shader_instance_param_get_bool(SEShaderInstance* shaderInstance, const char* name);
int se_shader_instance_param_get_int(SEShaderInstance* shaderInstance, const char* name);
float se_shader_instance_param_get_float(SEShaderInstance* shaderInstance, const char* name);
SEVector2 se_shader_instance_param_get_float2(SEShaderInstance* shaderInstance, const char* name);
SEVector3 se_shader_instance_param_get_float3(SEShaderInstance* shaderInstance, const char* name);
SEVector4 se_shader_instance_param_get_float4(SEShaderInstance* shaderInstance, const char* name);
