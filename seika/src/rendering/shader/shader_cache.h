#pragma once

#include "shader_instance_minimal.h"
#include "shader_instance.h"

// Holds references to all custom shaders

void se_shader_cache_initialize();
void se_shader_cache_finalize();

SEShaderInstanceId se_shader_cache_add_instance(SEShaderInstance* instance);
void se_shader_cache_remove_instance(SEShaderInstanceId instanceId);
SEShaderInstance* se_shader_cache_get_instance(SEShaderInstanceId instanceId);
SEShaderInstance* se_shader_cache_get_instance_checked(SEShaderInstanceId instanceId);
SEShaderInstanceId se_shader_cache_create_instance_and_add(const char* vertexPath, const char* fragmentPath);
