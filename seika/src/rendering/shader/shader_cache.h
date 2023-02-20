#pragma once

#include "shader_instance.h"

// Holds references to all custom shaders

void shader_cache_initialize();
void shader_cache_finalize();

void shader_cache_add_instance(ShaderInstance* instance, const char* id);
void shader_cache_remove_instance(const char* id);
ShaderInstance* shader_cache_get_instance(const char* id);
