#pragma once

#include <stdbool.h>

#include "glad/glad.h"

#include "../../math/se_math.h"

static const char* SE_SHADER_VERTEX_TYPE = "VERTEX";
static const char* SE_SHADER_FRAGMENT_TYPE = "FRAGMENT";
static const char* SE_SHADER_PROGRAM_TYPE = "PROGRAM";

typedef struct SEShader {
    GLuint id;
} SEShader;

SEShader* se_shader_compile_new_shader(const char* vertexSource, const char* fragmentSource);
void se_shader_destroy(SEShader* shader);
void se_shader_use(SEShader* shader);
void se_shader_set_bool(SEShader* shader, const char* name, bool value);
void se_shader_set_int(SEShader* shader, const char* name, int value);
void se_shader_set_float(SEShader* shader, const char* name, float value);
void se_shader_set_vec2_float(SEShader* shader, const char* name, float v1, float v2);
void se_shader_set_vec3_float(SEShader* shader, const char* name, float v1, float v2, float v3);
void se_shader_set_vec4_float(SEShader* shader, const char* name, float v1, float v2, float v3, float v4);
void se_shader_set_mat4_float(SEShader* shader, const char* name, mat4* value);
