#pragma once

#include "shader_instance_minimal.h"
#include "shader_instance.h"

// Holds references to all custom shaders

void shader_cache_initialize();
void shader_cache_finalize();

ShaderInstanceId shader_cache_add_instance(ShaderInstance* instance);
void shader_cache_remove_instance(ShaderInstanceId instanceId);
ShaderInstance* shader_cache_get_instance(ShaderInstanceId instanceId);
ShaderInstanceId shader_cache_create_instance_and_add(const char* vertexPath, const char* fragmentPath);
