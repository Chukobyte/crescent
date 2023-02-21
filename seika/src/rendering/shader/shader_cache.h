#pragma once

#include "shader_instance.h"

#define SHADER_CACHE_MAX_INSTANCES 100
#define SHADER_CACHE_INVALID_ID (SHADER_CACHE_MAX_INSTANCES + 1)

typedef uint32_t ShaderInstanceId;

// Holds references to all custom shaders

void shader_cache_initialize();
void shader_cache_finalize();

ShaderInstanceId shader_cache_add_instance(ShaderInstance* instance);
void shader_cache_remove_instance(ShaderInstanceId instanceId);
ShaderInstance* shader_cache_get_instance(ShaderInstanceId instanceId);
